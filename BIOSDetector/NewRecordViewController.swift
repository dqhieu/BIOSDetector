//
//  NewRecordViewController.swift
//  BIOSDetector
//
//  Created by Dinh Quang Hieu on 7/5/16.
//  Copyright © 2016 Dinh Quang Hieu. All rights reserved.
//

import UIKit

class NewRecordViewController: UIViewController {

    
    @IBOutlet weak var txtHeight: UITextField!
    @IBOutlet weak var txtWeight: UITextField!
    @IBOutlet weak var txtPulse: UITextField!
    @IBOutlet weak var txtSystolic: UITextField!
    @IBOutlet weak var txtDiastolic: UITextField!
    
    var strSender:String! = ""
    var userDefault:NSUserDefaults!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        userDefault = NSUserDefaults()
    }
    @IBAction func onTap(sender: AnyObject) {
        self.view.endEditing(true)
    }
    
    @IBAction func captureHeight(sender: AnyObject) {
        strSender = "height"
        captureImage(strSender)
    }
    
    @IBAction func captureWeight(sender: AnyObject) {
        strSender = "weight"
        captureImage(strSender)
    }
    
    @IBAction func capturePulse(sender: AnyObject) {
        strSender = "pulse"
        captureImage(strSender)
    }
    
    
    @IBAction func captureSystolic(sender: AnyObject) {
        strSender = "systolic"
        captureImage(strSender)
    }
    
    
    @IBAction func captureDiastolic(sender: AnyObject) {
        strSender = "diastolic"
        captureImage(strSender)
    }
    
    override func viewDidAppear(animated: Bool) {
        
        let height = userDefault.valueForKey("height") as? String
        let weight = userDefault.valueForKey("weight") as? String
        let pulse = userDefault.valueForKey("pulse") as? String
        let systolic = userDefault.valueForKey("systolic") as? String
        let diastolic = userDefault.valueForKey("diastolic") as? String
        
        if height != nil {
            txtHeight.text = height
        }
        if weight != nil && weight != "" {
            txtWeight.text = String(Double(weight!)! / 10)
        }
        if pulse != nil {
            txtPulse.text = pulse
        }
        if systolic != nil {
            txtSystolic.text = systolic
        }
        if diastolic != nil {
            txtDiastolic.text = diastolic
        }
        
    }
    
    override func viewDidDisappear(animated: Bool) {
        let height = txtHeight.text
        let weight = txtWeight.text
        let pulse = txtPulse.text
        let systolic = txtSystolic.text
        let diastolic = txtDiastolic.text
        
        userDefault.setValue(height, forKey: "height")
        userDefault.setValue(weight, forKey: "weight")
        userDefault.setValue(pulse, forKey: "pulse")
        userDefault.setValue(systolic, forKey: "systolic")
        userDefault.setValue(diastolic, forKey: "diastolic")
        userDefault.synchronize()
    }
    
    func captureImage(sender: String) {
        userDefault.setValue(strSender, forKey: "sender")
        userDefault.synchronize()
        performSegueWithIdentifier("segueCapture", sender: nil)
    }
    
}
