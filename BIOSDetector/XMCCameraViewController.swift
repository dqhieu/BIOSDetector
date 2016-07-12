//
//  XMCCameraViewController.swift
//  dojo-custom-camera
//
//  Created by David McGraw on 11/13/14.
//  Copyright (c) 2014 David McGraw. All rights reserved.
//

import UIKit
import AVFoundation

enum Status: Int {
    case Preview, Still, Error
}

class XMCCameraViewController: UIViewController, XMCCameraDelegate {

    @IBOutlet weak var cameraStill: UIImageView!
    @IBOutlet weak var cameraPreview: UIView!
    @IBOutlet weak var cameraStatus: UILabel!
    @IBOutlet weak var cameraCapture: UIButton!
    @IBOutlet weak var cameraCaptureShadow: UILabel!
    @IBOutlet weak var imgViewResult: UIImageView!
    @IBOutlet weak var background: UIImageView!
    
    @IBOutlet weak var lblResult: UILabel!
    var preview: AVCaptureVideoPreviewLayer?
    
    var camera: XMCCamera?
    var status: Status = .Preview
    
    var userDefault:NSUserDefaults!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        self.initializeCamera()
        userDefault = NSUserDefaults()
    }
    
    override func viewDidAppear(animated: Bool) {
        super.viewDidAppear(animated)
        self.establishVideoPreviewArea()
    }
    
    func initializeCamera() {
        self.cameraStatus.text = "Starting Camera"
        self.camera = XMCCamera(sender: self)
    }
    
    func establishVideoPreviewArea() {
        self.preview = AVCaptureVideoPreviewLayer(session: self.camera?.session)
        self.preview?.videoGravity = AVLayerVideoGravityResizeAspectFill
        self.preview?.frame = self.cameraPreview.alignmentRectForFrame(CGRect(x: 0, y: 0, width: 288, height: 144))
        self.preview?.cornerRadius = 0
        self.cameraPreview.layer.addSublayer(self.preview!)
    }
    
    // MARK: Button Actions
    
    @IBAction func captureFrame(sender: AnyObject) {
        if self.status == .Preview {
            self.cameraStatus.text = "Capturing Photo"
            /*
            UIView.animateWithDuration(0.1, animations: { () -> Void in
             
            })*/
            self.cameraPreview.alpha = 0.0;
            self.cameraStatus.alpha = 1.0
            self.camera?.captureStillImage({ (image) -> Void in
                if image != nil {
                    self.cameraStill.image = image;
                    
                    /*
                
                    UIView.animateWithDuration(0.1, animations: { () -> Void in
                    })*/
                    
                    self.cameraStill.alpha = 1.0;
                    self.cameraStatus.alpha = 0.0;
                    self.status = .Still
                    
                    self.onStartDetecting(image!);
                    
                } else {
                    self.cameraStatus.text = "Uh oh! Something went wrong. Try it again."
                    self.status = .Error
                }
                
                self.cameraCapture.setTitle("Reset", forState: UIControlState.Normal)
            })
        } else if self.status == .Still || self.status == .Error {
            background.hidden = false
            UIView.animateWithDuration(0, animations: { () -> Void in
                self.cameraStill.alpha = 0.0;
                self.cameraStatus.alpha = 0.0;
                self.cameraPreview.alpha = 1.0;
                self.cameraCapture.setTitle("Capture", forState: UIControlState.Normal)
            }, completion: { (done) -> Void in
                self.cameraStill.image = nil;
                self.status = .Preview
            })
        }
    }
    
    func onStartDetecting(image:UIImage) {
        background.hidden = true
        let rotated = image.imageRotatedByDegrees(90, flip: false)
        let resized = resizeImage(rotated, targetSize: CGSizeMake(900, 1200))
        let cropped = cropImage(resized, size: CGSize(width: 450, height: 225))
        self.cameraStill.image = cropped
        self.cameraStatus.text = "Detecting..."
        dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0)) {
            
            //let imageResult = CVWrapper.processImageWithOpenCV(UIImage(named: "1762.png"))
            let imageResult = CVWrapper.processImageWithOpenCV(cropped)
            
            dispatch_async(dispatch_get_main_queue()) {
                
                //self.imgViewResult.image = imageResult
                self.lblResult.text = imageResult
                let strSender = self.userDefault.valueForKey("sender") as! String
                self.userDefault.setValue(imageResult, forKey: strSender)
                self.userDefault.synchronize()
                self.camera?.stopCamera()
                self.performSegueWithIdentifier("segueBack", sender: nil)
            }
        }
    }
    
    // MARK: Camera Delegate
    
    func cameraSessionConfigurationDidComplete() {
        self.camera?.startCamera()
    }
    
    func cameraSessionDidBegin() {
        self.cameraStatus.text = ""
        UIView.animateWithDuration(0, animations: { () -> Void in
            self.cameraStatus.alpha = 0.0
            self.cameraPreview.alpha = 1.0
            self.cameraCapture.alpha = 1.0
            self.cameraCaptureShadow.alpha = 0.4;
        })
    }
    
    func cameraSessionDidStop() {
        self.cameraStatus.text = "Camera Stopped"
        UIView.animateWithDuration(0, animations: { () -> Void in
            self.cameraStatus.alpha = 1.0
            self.cameraPreview.alpha = 0.0
        })
    }
    
    func cropImage(screenshot: UIImage, size: CGSize) -> UIImage {
        let crop = CGRectMake(220, 490, size.width, size.height)
        let cgImage = CGImageCreateWithImageInRect(screenshot.CGImage, crop)
        let image: UIImage = UIImage(CGImage: cgImage!)
        return image
    }
    
    func resizeImage(image: UIImage, targetSize: CGSize) -> UIImage {
        let newSize = CGSizeMake(targetSize.width, targetSize.height)
        // This is the rect that we've calculated out and this is what is actually used below
        let rect = CGRectMake(0, 0, newSize.width, newSize.height)
        
        // Actually do the resizing to the rect using the ImageContext stuff
        UIGraphicsBeginImageContextWithOptions(newSize, false, 1.0)
        image.drawInRect(rect)
        let newImage = UIGraphicsGetImageFromCurrentImageContext()
        UIGraphicsEndImageContext()
        
        return newImage
    }
}

extension UIImage {
    public func imageRotatedByDegrees(degrees: CGFloat, flip: Bool) -> UIImage {
        let radiansToDegrees: (CGFloat) -> CGFloat = {
            return $0 * (180.0 / CGFloat(M_PI))
        }
        let degreesToRadians: (CGFloat) -> CGFloat = {
            return $0 / 180.0 * CGFloat(M_PI)
        }
        
        // calculate the size of the rotated view's containing box for our drawing space
        let rotatedViewBox = UIView(frame: CGRect(origin: CGPointZero, size: size))
        let t = CGAffineTransformMakeRotation(degreesToRadians(degrees));
        rotatedViewBox.transform = t
        let rotatedSize = rotatedViewBox.frame.size
        
        // Create the bitmap context
        UIGraphicsBeginImageContext(rotatedSize)
        let bitmap = UIGraphicsGetCurrentContext()
        
        // Move the origin to the middle of the image so we will rotate and scale around the center.
        CGContextTranslateCTM(bitmap, rotatedSize.width / 2.0, rotatedSize.height / 2.0);
        
        //   // Rotate the image context
        CGContextRotateCTM(bitmap, degreesToRadians(degrees));
        
        // Now, draw the rotated/scaled image into the context
        var yFlip: CGFloat
        
        if(flip){
            yFlip = CGFloat(-1.0)
        } else {
            yFlip = CGFloat(1.0)
        }
        
        CGContextScaleCTM(bitmap, yFlip, -1.0)
        CGContextDrawImage(bitmap, CGRectMake(-size.width / 2, -size.height / 2, size.width, size.height), CGImage)
        
        let newImage = UIGraphicsGetImageFromCurrentImageContext()
        UIGraphicsEndImageContext()
        
        return newImage
    }
}


