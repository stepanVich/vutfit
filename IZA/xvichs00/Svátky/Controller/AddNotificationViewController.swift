//
//  AddNotificationViewController.swift
//  Svátky
//
//  Created by Štěpán Vích on 06/05/2018.
//  Copyright © 2018 Štěpán Vích. All rights reserved.
//

import UIKit
import os.log

class AddNotificationViewController: UIViewController, UITextFieldDelegate {
    var notification: Notification?;
    @IBOutlet weak var textFieldName: UITextField!
    @IBOutlet weak var saveButton: UIBarButtonItem!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        textFieldName.delegate = self;
        textFieldName.addTarget(self, action:#selector(textFieldDidChange(textField:)), for: UIControlEvents.editingChanged)
        saveButton.isEnabled = false
        // Do any additional setup after loading the view.
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    

    
    // MARK: - Navigation

    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        // Get the new view controller using segue.destinationViewController.
        // Pass the selected object to the new view controller.
        super.prepare(for: segue, sender: sender)
        guard let button = sender as? UIBarButtonItem, button === saveButton else {
            os_log("The save button was not pressed, cancelling", log: OSLog.default, type: .debug)
            return
        }
        var name = textFieldName.text ?? ""
        name = name.prefix(1).uppercased() + name.dropFirst().lowercased();
        notification = Notification(name: name);
    }
 

    //MARK: UITextFieldDelegate
    func textFieldShouldReturn(_ textField: UITextField) -> Bool {
        textField.resignFirstResponder()
        return true;
    }
    
    @objc public func textFieldDidChange(textField: UITextField) {
        if(Names.nameToDate(name: textField.text!) != nil) {
            saveButton.isEnabled = true;
        } else {
            saveButton.isEnabled = false;
        }
    }
    
}
