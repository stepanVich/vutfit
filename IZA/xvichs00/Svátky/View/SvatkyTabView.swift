//
//  SvatkyTabView.swift
//  Svátky
//
//  Created by Štěpán Vích on 03/05/2018.
//  Copyright © 2018 Štěpán Vích. All rights reserved.
//

import Foundation
import UIKit


class SvatkyTabView: UIViewController {
    
    
    @IBOutlet weak var dateLabel: UILabel!
    
    @IBOutlet weak var currentDayNameLabel: UILabel!
    @IBOutlet weak var yesterdayDayNameLabel: UILabel!
    @IBOutlet weak var tomorrowDayNameLabel: UILabel!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
        
        // Vypis aktualniho data
        let date = Date()
        
        let yesterday = Calendar.current.date(byAdding: .day, value: -1, to: Date())
        let tomorrow = Calendar.current.date(byAdding: .day, value: 1, to: Date())
        
        let formatter = DateFormatter()
        formatter.dateFormat = "dd.MM.yyyy";
        let dateValue = formatter.string(from: date)
        dateLabel.text = dateValue;
        
        // Ulozeni aktualniho dne
        formatter.dateFormat = "dd";
        let currentDay = Int(formatter.string(from: date));
        let yesterdayDay = Int(formatter.string(from: yesterday!));
        let tomorrowDay = Int(formatter.string(from: tomorrow!));

        // Ulozeni aktualniho mesice
        formatter.dateFormat = "MM";
        let currentMonth = Int(formatter.string(from:date));
        let yesterdayMonth = Int(formatter.string(from: yesterday!));
        let tomorrowMonth = Int(formatter.string(from: tomorrow!));
        
        // Vypis aktualnih jmenin
        // Dnesnich
        currentDayNameLabel.text = Names.dateToName(day: currentDay!, month:currentMonth!);
        // Vcerejsich
        yesterdayDayNameLabel.text = Names.dateToName(day: yesterdayDay!, month: yesterdayMonth!);
        // Zitrejsich
        tomorrowDayNameLabel.text = Names.dateToName(day: tomorrowDay!, month: tomorrowMonth!);
        
    }
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    
}
    
    

