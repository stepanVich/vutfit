//
//  NotificationTableViewController.swift
//  Svátky
//
//  Created by Štěpán Vích on 05/05/2018.
//  Copyright © 2018 Štěpán Vích. All rights reserved.
//

import UIKit
import UserNotifications
import os.log

class NotificationTableViewController: UITableViewController {
    var notifications = [Notification]()

    override func viewDidLoad() {
        super.viewDidLoad()
        
        //self.loadNotifications();
        navigationItem.leftBarButtonItem = editButtonItem;
        
        if let savedNotifications = loadNotifications() {
            notifications += savedNotifications
        }

        
        // Uncomment the following line to preserve selection between presentations
        // self.clearsSelectionOnViewWillAppear = false

        // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
        // self.navigationItem.rightBarButtonItem = self.editButtonItem
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }

    // MARK: - Table view data source

    override func numberOfSections(in tableView: UITableView) -> Int {
        return 1
    }

    override func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return self.notifications.count
    }

    
    override func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cellIdentifier = "NameNotificationTableViewCellId"
        guard let cell = tableView.dequeueReusableCell(withIdentifier: cellIdentifier, for: indexPath) as? NameNotificationTableViewCell  else {
            fatalError("Cell can't be instantied.")
        }
        let date = Date();
        let dateFormatter = DateFormatter();
        dateFormatter.dateFormat = "MM";
        let currentMonth = Int(dateFormatter.string(from: date))!;
        dateFormatter.dateFormat = "dd";
        let currentDay = Int(dateFormatter.string(from: date))!;
        dateFormatter.dateFormat = "yyyy";
        let currentYear = Int(dateFormatter.string(from: date))!;
        var notificationYear = currentYear;
        
        let notification = self.notifications[indexPath.row];
        
        cell.cellName.text = notification.getName();
        let nameDay = Names.nameToDate(name: notification.getName())
        if(nameDay == nil) {
            cell.cellDate.text = "--";
            return cell;
        }
        // Calculate next name day
        // Add or not year to next year
        if currentMonth > nameDay!.month {
            notificationYear += 1;
        } else if currentMonth == nameDay!.month && currentDay > nameDay!.day {
            notificationYear += 1;
        }
        cell.cellDate.text = "\(nameDay!.day). \(dateFormatter.monthSymbols[nameDay!.month - 1]) \(notificationYear)";
        return cell;
    }
    

    
    // Override to support conditional editing of the table view.
    override func tableView(_ tableView: UITableView, canEditRowAt indexPath: IndexPath) -> Bool {
        // Return false if you do not want the specified item to be editable.
        return true
    }
 

    
    // Override to support editing the table view.
    override func tableView(_ tableView: UITableView, commit editingStyle: UITableViewCellEditingStyle, forRowAt indexPath: IndexPath) {
        if editingStyle == .delete {
            // Delete the row from the data source
            let n = notifications.remove(at: indexPath.row)
            saveNotifications();
            // Odstran notifikaci z planovace notifikaci
            
             UNUserNotificationCenter.current().removePendingNotificationRequests(withIdentifiers: ["svatky.\(n.getName())"])
            
            tableView.deleteRows(at: [indexPath], with: .fade)
        } else if editingStyle == .insert {
            // Create a new instance of the appropriate class, insert it into the array, and add a new row to the table view
        }    
    }
    

    /*
    // Override to support rearranging the table view.
    override func tableView(_ tableView: UITableView, moveRowAt fromIndexPath: IndexPath, to: IndexPath) {

    }
    */

    /*
    // Override to support conditional rearranging of the table view.
    override func tableView(_ tableView: UITableView, canMoveRowAt indexPath: IndexPath) -> Bool {
        // Return false if you do not want the item to be re-orderable.
        return true
    }
    */

    /*
    // MARK: - Navigation

    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        // Get the new view controller using segue.destinationViewController.
        // Pass the selected object to the new view controller.
    }
    */

    //MARK: Actions
    @IBAction func unwindToNotificationList(sender: UIStoryboardSegue) {
        if let sourceViewController = sender.source as? AddNotificationViewController, let notification = sourceViewController.notification {
            // Add a new Notification.
            let newIndexPath = IndexPath(row: notifications.count, section: 0)
            
            let notifyCenter = UNUserNotificationCenter.current()
            notifyCenter.requestAuthorization(options: [.alert, .sound]) { (granted, error) in
                // Enable or disable features based on authorization.
                print("granted: \(granted), error: \(error)");
            }
            
            // Zkontroluj jestli se jiz notifikace se stejnym jmenem nenachazi v seznamu
            for n in notifications {
                if(n.getName() == notification.getName()) {
                    return;
                }
            }
            
            // Pridat notifikaci do seznamu + do operacniho systemu
            notifications.append(notification)
            // Ulozeni notifikace do systemu
            let content = UNMutableNotificationContent()
            content.title = NSString.localizedUserNotificationString(forKey: "\(notification.getName()) má brzo jmeniny!", arguments: nil)
            content.body = NSString.localizedUserNotificationString(forKey: "Pošlete zprávu nebo kupte kytku!",
                                                                    arguments: nil)
            
            // Configure the trigger for a 7am wakeup.
            var dateInfo = DateComponents()
            dateInfo.hour = 7
            dateInfo.day = Names.nameToDate(name: notification.getName())!.day;
            dateInfo.month = Names.nameToDate(name: notification.getName())!.month;
            let trigger = UNCalendarNotificationTrigger(dateMatching: dateInfo, repeats: true)
            // Create the request object.
            let request = UNNotificationRequest(identifier: "svatky.\(notification.getName())", content: content, trigger: trigger)
            
            notifyCenter.add(request) { (error : Error?) in
                if let theError = error {
                    print(theError.localizedDescription)
                }
            }
            
            tableView.insertRows(at: [newIndexPath], with: .automatic)
            
        }
        saveNotifications();
    }
    //MARK: Private Methods
    private func saveNotifications() {
        let isSuccessfulSave = NSKeyedArchiver.archiveRootObject(notifications, toFile: Notification.ArchiveURL.path)
        if isSuccessfulSave {
            os_log("Notifications successfully saved.", log: OSLog.default, type: .debug)
        } else {
            os_log("Failed to save notifications...", log: OSLog.default, type: .error)
        }
    }
    
    private func loadNotifications() -> [Notification]?  {
        return NSKeyedUnarchiver.unarchiveObject(withFile: Notification.ArchiveURL.path) as? [Notification]
    }
}
