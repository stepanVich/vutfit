//
//  NameNotificationTableViewCell.swift
//  Svátky
//
//  Created by Štěpán Vích on 05/05/2018.
//  Copyright © 2018 Štěpán Vích. All rights reserved.
//

import UIKit

class NameNotificationTableViewCell: UITableViewCell {

    @IBOutlet weak var cellName: UILabel!
    @IBOutlet weak var cellDate: UILabel!
    @IBOutlet weak var cellAlarm: UIButton!
    
    override func awakeFromNib() {
        super.awakeFromNib()
        // Initialization code
    }

    override func setSelected(_ selected: Bool, animated: Bool) {
        //super.setSelected(selected, animated: animated)

        // Configure the view for the selected state
        //cell.selectionStyle = .none
    }

}
