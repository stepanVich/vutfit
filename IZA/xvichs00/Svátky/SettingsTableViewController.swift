//
//  SettingsTableViewController.swift
//  Svátky
//
//  Created by Štěpán Vích on 07/05/2018.
//  Copyright © 2018 Štěpán Vích. All rights reserved.
//

import UIKit
import UserNotifications
import os.log

class SettingsTableViewController: UITableViewController {
    
    var settings = [Settings]()
    
    
    override func viewDidLoad() {
        super.viewDidLoad()

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
        // #warning Incomplete implementation, return the number of sections
        return 1
    }

    override func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        // Oznac aktivni bunku
        // #warning Incomplete implementation, return the number of rows
        if let savedSettings = loadSettings() {
            settings = savedSettings
        } else {
            settings = [Settings(notificationDayOffset: "0", notificationHour: "7")];
            saveSettings();
        }
        if settings.count > 0 {
            if settings[0].getNotificationHour() == 7 && settings[0].getNotificationDayOffset() == 0 {
                tableView.visibleCells[0].accessoryType = .checkmark
            }
            if settings[0].getNotificationHour() == 13 && settings[0].getNotificationDayOffset() == 0 {
                tableView.visibleCells[1].accessoryType = .checkmark
            }
            
            if settings[0].getNotificationDayOffset() == -1 {
                tableView.visibleCells[2].accessoryType = .checkmark
            }
            
            if settings[0].getNotificationDayOffset() == -2 {
                tableView.visibleCells[3].accessoryType = .checkmark
            }
            
            if settings[0].getNotificationDayOffset() == -4 {
                tableView.visibleCells[4].accessoryType = .checkmark
            }
        }
        return 5
    }

    override func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        //Your action here
        for cell in tableView.visibleCells {
            cell.accessoryType = .none;
        }
        tableView.cellForRow(at: indexPath)?.accessoryType = .checkmark
        
        // Ulozeni nastaveni a preorganizovani notifikace
        switch(indexPath.row) {
            case 0:
                settings[0].setNotificationHour(7)
                settings[0].setNotificationDayOffset(0)
            case 1:
                settings[0].setNotificationHour(13)
                settings[0].setNotificationDayOffset(0)
            case 2:
                settings[0].setNotificationHour(7)
                settings[0].setNotificationDayOffset(-1)
            case 3:
                settings[0].setNotificationHour(7)
                settings[0].setNotificationDayOffset(-2)
            case 4:
                settings[0].setNotificationHour(7)
                settings[0].setNotificationDayOffset(-4)
            default:
                settings[0].setNotificationHour(7)
                settings[0].setNotificationDayOffset(0)
        }
        // save settings
        saveSettings();
        // Ulozeni nastaveni
        
        
        // Preorganizace
    }
    
   /* override func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCell(withIdentifier: "reuseIdentifier", for: indexPath)

        // Configure the cell...

        return cell
    }*/
    

    /*
    // Override to support conditional editing of the table view.
    override func tableView(_ tableView: UITableView, canEditRowAt indexPath: IndexPath) -> Bool {
        // Return false if you do not want the specified item to be editable.
        return true
    }
    */

    /*
    // Override to support editing the table view.
    override func tableView(_ tableView: UITableView, commit editingStyle: UITableViewCellEditingStyle, forRowAt indexPath: IndexPath) {
        if editingStyle == .delete {
            // Delete the row from the data source
            tableView.deleteRows(at: [indexPath], with: .fade)
        } else if editingStyle == .insert {
            // Create a new instance of the appropriate class, insert it into the array, and add a new row to the table view
        }    
    }
    */

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
    
    //MARK: Private Methods
    private func saveSettings() {
        let isSuccessfulSave = NSKeyedArchiver.archiveRootObject(settings, toFile: Settings.ArchiveURL.path)
        if isSuccessfulSave {
            os_log("Settings successfully saved.", log: OSLog.default, type: .debug)
        } else {
            os_log("Unable to save settings...", log: OSLog.default, type: .error)
        }
    }
    
    private func loadSettings() -> [Settings]?  {
        return NSKeyedUnarchiver.unarchiveObject(withFile: Settings.ArchiveURL.path) as? [Settings]
    }
    

}
