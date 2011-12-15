//Copyright (c) 2011 Daniel Dickison
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in
//all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//THE SOFTWARE.

@protocol FlipsideViewControllerDelegate;
@class TrackPadControl;
@class MultiStateButton;

@interface FlipsideViewController : UIViewController <UITextFieldDelegate>

@property (nonatomic, assign) id <FlipsideViewControllerDelegate> delegate;
@property (nonatomic, assign) IBOutlet UITextField *ipField;
@property (nonatomic, assign) IBOutlet UITextField *portField;
@property (nonatomic, assign) IBOutlet MultiStateButton *autoCenterLeft;
@property (nonatomic, assign) IBOutlet MultiStateButton *autoCenterRight;
@property (nonatomic, assign) IBOutlet UISwitch *centerOnHoldSwitch;
@property (nonatomic, assign) IBOutlet UISegmentedControl *autoHoldTrigger;
@property (nonatomic, assign) IBOutlet UIBarButtonItem *doneButton;
@property (nonatomic, assign) IBOutlet UIBarButtonItem *helpButton;
- (IBAction)done;
- (IBAction)ipFieldChanged;
- (IBAction)portFieldChanged;
- (IBAction)showHelp;
- (IBAction)showBonjour;
- (IBAction)centerOnHoldChanged;
- (IBAction)autoHoldTriggerChanged;
- (IBAction)autoCenterLeftChanged;
- (IBAction)autoCenterRightChanged;


@end


@protocol FlipsideViewControllerDelegate
- (void)flipsideViewControllerDidFinish:(FlipsideViewController *)controller;
@end

