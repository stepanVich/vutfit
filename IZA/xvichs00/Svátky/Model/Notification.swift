//
//  Notification.swift
//  Svátky
//
//  Created by Štěpán Vích on 05/05/2018.
//  Copyright © 2018 Štěpán Vích. All rights reserved.
//

import UIKit
import os.log

// Model data class
class Notification: NSObject, NSCoding {
    private let name:String;
    private var isTurnOn:Bool;
    init(name:String, isTurnOn:Bool = false) {
        self.name = name;
        self.isTurnOn = isTurnOn;
    }
    
    public func turnOn() -> Void {
        self.isTurnOn = true;
    }
    
    public func turnOff() -> Void {
        self.isTurnOn = false;
    }
    
    public func getName() -> String {
        return self.name;
    }
    //MARK: NSCoding
    func encode(with aCoder: NSCoder) {
        aCoder.encode(name, forKey: PropertyKeyNotification.name)
    }
    required convenience init?(coder aDecoder: NSCoder) {
        
        // The name is required. If we cannot decode a name string, the initializer should fail.
        guard let name = aDecoder.decodeObject(forKey: PropertyKeyNotification.name) as? String else {
            os_log("Unable to decode the name for a Notification object.", log: OSLog.default, type: .debug)
            return nil
        }
        
        // Must call designated initializer.
        self.init(name: name)
        
    }
    //MARK: Archiving Paths
    
    static let DocumentsDirectory = FileManager().urls(for: .documentDirectory, in: .userDomainMask).first!
    static let ArchiveURL = DocumentsDirectory.appendingPathComponent("notifications")
}

//MARK: Types

struct PropertyKeyNotification {
    static let name = "name"
}
