#include "gamepad.h"

//#define MT_GAMEPAD_DEBUG


// Button event queue length
static const unsigned int QUEUE_LENGTH = 25;

MT_HIDGamePad::MT_HIDGamePad()
#ifdef MT_GAMEPAD_USE_WX
  : myJoystick(wxJOYSTICK1)
#endif
{
    StatusGood = false;
    ButtonStates = 0;
    Hat = 0;
    X = Y = Z = W = 0;
    Xf = Yf = Zf = Wf = 0;
    U2 = U3 = U4 = 0;
  
#ifdef MT_GAMEPAD_MAC
    xcenter = ycenter = zcenter = wcenter = 0;
    xnorm = ynorm = znorm = wnorm = 1.0/128.0;
#elif defined(MT_GAMEPAD_USE_WX)
    float min = (float) myJoystick.GetXMin();
    float max = (float) myJoystick.GetXMax();
    xcenter = floor(0.5*(min + max));
    xnorm = 2.0/(max - min);
  
    min = (float) myJoystick.GetYMin();
    max = (float) myJoystick.GetYMax();
    ycenter = floor(0.5*(min + max));
    ynorm = -2.0/(max - min);  // uninvert y so up is positive

    /* On the Thrustmaster Dual Analog 3, Rudder and Z are the 
       horizontal and vertical axes of the right stick. */
    min = (float) myJoystick.GetRudderMin();
    max = (float) myJoystick.GetRudderMax();
    wcenter = floor(0.5*(min + max));
    wnorm = 2.0/(max - min);

    min = (float) myJoystick.GetZMin();
    max = (float) myJoystick.GetZMax();
    zcenter = floor(0.5*(min + max));
    znorm = -2.0/(max - min);  // uninvert z so up is positive
  
#endif
  
}

MT_HIDGamePad::~MT_HIDGamePad()
{
  
    Disconnect();
  
};

void MT_HIDGamePad::Disconnect()
{
#ifdef MT_GAMEPAD_MAC
    if(StatusGood)
    {
        (*hidDeviceInterface)->Release(hidDeviceInterface);
        IOObjectRelease(hidObjectIterator);
    
        // Clean up the queue
        (*ButtonQueue)->stop(ButtonQueue);
        //(*ButtonQueue)->Release(ButtonQueue);
        (*ButtonQueue)->dispose(ButtonQueue);
        StatusGood = false;
    }
#endif
}

bool MT_HIDGamePad::GetStatus()
{
#ifdef MT_GAMEPAD_MAC
    return StatusGood;
#elif defined(MT_GAMEPAD_USE_WX)
    return StatusGood && myJoystick.IsOk();
#else /* no gamepad! */
    return false;
#endif
}

unsigned int MT_HIDGamePad::Init()
{
  
#ifdef MT_GAMEPAD_MAC
    if(FindHIDDevices()){
        fprintf(stderr,"No matching HID class devices found.\n");
        fflush(stderr);
        return 1;
    } else if(FindGamePad()){
        fprintf(stderr,"No gamepad found.\n");
        fflush(stderr);
        return 1;
    } else if(CreateHIDDeviceInterface(&hidDeviceInterface)) {
        fprintf(stderr,"Couldn't create HID class device interface.\n");
        fflush(stderr);
        return 1;
    } else {
        gpCookies = getHIDCookies((IOHIDDeviceInterface122 **)hidDeviceInterface);
        if(IOObjectRelease(hidDevice)){
            fprintf(stderr,"Error releasing HID device.\n");
            fflush(stderr);
            return 1;
        }
        if((*hidDeviceInterface)->open(hidDeviceInterface,0)){
            fprintf(stderr,"Error opening HID device.\n");
            fflush(stderr);
            return 1;
        }
    }
  
    // Set up the button queue
    ButtonQueue = (*hidDeviceInterface)->allocQueue (hidDeviceInterface);
    if(!ButtonQueue){
        fprintf(stderr,"Could not create button queue.\n");
        fflush(stderr);
        return 1;
    }
  
  
    (*ButtonQueue)->create(ButtonQueue,8,QUEUE_LENGTH);
    (*ButtonQueue)->addElement(ButtonQueue, gpCookies->gButton1Cookie, 0);
    (*ButtonQueue)->addElement(ButtonQueue, gpCookies->gButton2Cookie, 0);
    (*ButtonQueue)->addElement(ButtonQueue, gpCookies->gButton3Cookie, 0);
    (*ButtonQueue)->addElement(ButtonQueue, gpCookies->gButton4Cookie, 0);
    (*ButtonQueue)->addElement(ButtonQueue, gpCookies->gButton5Cookie, 0);
    (*ButtonQueue)->addElement(ButtonQueue, gpCookies->gButton6Cookie, 0);
    (*ButtonQueue)->addElement(ButtonQueue, gpCookies->gButton7Cookie, 0);
    (*ButtonQueue)->addElement(ButtonQueue, gpCookies->gButton8Cookie, 0);
    (*ButtonQueue)->addElement(ButtonQueue, gpCookies->gButton9Cookie, 0);
    (*ButtonQueue)->addElement(ButtonQueue, gpCookies->gButtonaCookie, 0);
    (*ButtonQueue)->addElement(ButtonQueue, gpCookies->gButtonbCookie, 0);
    (*ButtonQueue)->addElement(ButtonQueue, gpCookies->gButtoncCookie, 0);
  
    (*ButtonQueue)->start(ButtonQueue);

    xcenter = PollXAxis();
    ycenter = PollYAxis();
  
#elif defined MT_GAMEPAD_USE_WX
    
    if(!myJoystick.IsOk())
    {
        //wxMessageBox(_T("No joystick detected."));
        return 1;
    }  

#endif
  
    StatusGood = true;
  
    return 0;

}

#ifdef MT_GAMEPAD_MAC
unsigned int MT_HIDGamePad::FindHIDDevices()
{
    CFMutableDictionaryRef hidMatchDictionary = NULL;
    IOReturn ioReturnValue = kIOReturnSuccess;
    Boolean noMatchingDevices = false;

    UInt32 potential_usages[] = {kHIDUsage_GD_GamePad, kHIDUsage_GD_Joystick};
    unsigned int n_usages = 2;  // # of elements in potential_usages
  
    /* Code borrowed from SDL -
       https://stage.maemo.org/svn/maemo/projects/haf/branches/libsdl1.2/xic-never-released/src/joystick/darwin/SDL_sysjoystick.c
       which is based on Apple's HID Manager Basics code -
       http://developer.apple.com/mac/library/samplecode/HID_Manager_Basics/listing3.html
       NOTE This will only search for GamePads, not Joysticks.  Not
       sure how to search for both.*/
    /* Add key for device type (joystick, in this case) to refine the
     * matching dictionary. */
    /* Modified DTS on 11/17/10 to look for GamePads OR JoySticks */
    UInt32 usagePage = kHIDPage_GenericDesktop;
    UInt32 usage = 0;
    CFNumberRef refUsage = NULL, refUsagePage = NULL;

    for(unsigned int i = 0; i < n_usages; i++)
    {
        usage = potential_usages[i];

        // Set up a matching dictionary to search the I/O Registry by class
        // name for all HID class devices
        hidMatchDictionary = IOServiceMatching(kIOHIDDeviceKey);
        
        refUsage = CFNumberCreate (kCFAllocatorDefault,
                                   kCFNumberIntType,
                                   &usage);
        CFDictionarySetValue(hidMatchDictionary,
                             CFSTR (kIOHIDPrimaryUsageKey),
                             refUsage);
        refUsagePage = CFNumberCreate(kCFAllocatorDefault,
                                      kCFNumberIntType,
                                      &usagePage);
        CFDictionarySetValue(hidMatchDictionary,
                             CFSTR (kIOHIDPrimaryUsagePageKey),
                             refUsagePage);  
        /* End of SDL/HID Manager Basics code */
  
        // Now search I/O Registry for matching devices.
        ioReturnValue = IOServiceGetMatchingServices(kIOMasterPortDefault, 
                                                     hidMatchDictionary,
                                                     &hidObjectIterator);

        noMatchingDevices = ((ioReturnValue != kIOReturnSuccess) 
                             | !(hidObjectIterator));
        
        if(noMatchingDevices == 0)
        {
            break;
        }
    }

    // IOServiceGetMatchingServices consumes a reference to the
    //   dictionary, so we don't need to release the dictionary ref.
    hidMatchDictionary = NULL;

    return noMatchingDevices;
}

unsigned int MT_HIDGamePad::FindGamePad()
{
    IOReturn ioReturnValue = kIOReturnSuccess;
    io_name_t className;

    while( (hidDevice = IOIteratorNext(hidObjectIterator)) )
    {
        ioReturnValue = IOObjectGetClass(hidDevice,className);
        if(ioReturnValue != kIOReturnSuccess){
            fprintf(stderr,"Warning:  Could not get HID class name.\n");
            fflush(stderr);
        }
        if(strcmp(className,"IOUSBHIDDriver") == 0)
            return 0;
    }
    return 0;
}

unsigned int MT_HIDGamePad::CreateHIDDeviceInterface(IOHIDDeviceInterface*** hidDeviceInterface)
{
    IOCFPlugInInterface** plugInInterface = NULL;
    HRESULT  plugInResult = S_OK;
    SInt32   score = 0;
    IOReturn  ioReturnValue = kIOReturnSuccess;

    ioReturnValue = IOCreatePlugInInterfaceForService(hidDevice,
                                                      kIOHIDDeviceUserClientTypeID,
                                                      kIOCFPlugInInterfaceID,
                                                      &plugInInterface,
                                                      &score);

    if (ioReturnValue == kIOReturnSuccess)
    {
        //Call a method of the intermediate plug-in to create the device 
        //interface
        plugInResult = (*plugInInterface)->QueryInterface(plugInInterface,
                                                          CFUUIDGetUUIDBytes(kIOHIDDeviceInterfaceID),
                                                          (LPVOID*) hidDeviceInterface);

        (*plugInInterface)->Release(plugInInterface);

        return plugInResult != S_OK;
    } else {
        return 1;
    }
}

cookie_struct_t MT_HIDGamePad::getHIDCookies(IOHIDDeviceInterface122 **handle)
{
    cookie_struct_t cookies = (cookie_struct_t )malloc(sizeof(*cookies));
    memset(cookies, 0, sizeof(*cookies));
    CFTypeRef object;
    long  number;
    IOHIDElementCookie  cookie;
    long  usage;
    long  usagePage;
    CFArrayRef  elements;
    CFDictionaryRef  element;
    IOReturn success;

    if (!handle || !(*handle)) return cookies;

    // Copy all elements, since we're grabbing most of the elements
    // for this device anyway, and thus, it's faster to iterate them
    // ourselves. When grabbing only one or two elements, a matching
    // dictionary should be passed in here instead of NULL.
    success = (*handle)->copyMatchingElements(handle, NULL, &elements);

    if (success == kIOReturnSuccess) {
        CFIndex i;
        for (i=0; i<CFArrayGetCount(elements); i++)
        {
            element = (CFDictionaryRef )CFArrayGetValueAtIndex(elements, i);
            // printf("GOT ELEMENT.\n");

            //Get cookie
            object = (CFDictionaryGetValue(element, CFSTR(kIOHIDElementCookieKey)));
            if (object == 0 || CFGetTypeID(object) != CFNumberGetTypeID())
                continue;
            if(!CFNumberGetValue((CFNumberRef) object, kCFNumberLongType, &number))
                continue;
            cookie = (IOHIDElementCookie) number;

            //Get usage
            object = CFDictionaryGetValue(element, CFSTR(kIOHIDElementUsageKey));
            if (object == 0 || CFGetTypeID(object) != CFNumberGetTypeID())
                continue;
            if (!CFNumberGetValue((CFNumberRef) object, kCFNumberLongType, &number))
                continue;
            usage = number;

            //Get usage page
            object = CFDictionaryGetValue(element,CFSTR(kIOHIDElementUsagePageKey));
            if (object == 0 || CFGetTypeID(object) != CFNumberGetTypeID())
                continue;
            if (!CFNumberGetValue((CFNumberRef) object, kCFNumberLongType, &number))
                continue;
            usagePage = number;

#ifdef MT_GAMEPAD_DEBUG
            // Debugging information - prints out the pairs we get
            printf("%lx, %lx\n",usage,usagePage);
#endif

            //Check for axes
            if (usage == 0x30 && usagePage == 0x01)
                cookies->gXAxisCookie = cookie;
            else if (usage == 0x31 && usagePage == 0x01)
                cookies->gYAxisCookie = cookie;
            else if (usage == 0x35 && usagePage == 0x01)
                cookies->gWAxisCookie = cookie;
            else if (usage == 0x36 && usagePage == 0x01)
                cookies->gZAxisCookie = cookie;

            //Check for buttons
            else if (usage == 0x01 && usagePage == 0x09)
                cookies->gButton1Cookie = cookie;
            else if (usage == 0x02 && usagePage == 0x09)
                cookies->gButton2Cookie = cookie;
            else if (usage == 0x03 && usagePage == 0x09)
                cookies->gButton3Cookie = cookie;
            else if (usage == 0x04 && usagePage == 0x09)
                cookies->gButton4Cookie = cookie;
            else if (usage == 0x05 && usagePage == 0x09)
                cookies->gButton5Cookie = cookie;
            else if (usage == 0x06 && usagePage == 0x09)
                cookies->gButton6Cookie = cookie;
            else if (usage == 0x07 && usagePage == 0x09)
                cookies->gButton7Cookie = cookie;
            else if (usage == 0x08 && usagePage == 0x09)
                cookies->gButton8Cookie = cookie;
            else if (usage == 0x09 && usagePage == 0x09)
                cookies->gButton9Cookie = cookie;
            else if (usage == 0x0a && usagePage == 0x09)
                cookies->gButtonaCookie = cookie;
            // b and c don't appear to be used....
            else if (usage == 0x0b && usagePage == 0x09)
                cookies->gButtonbCookie = cookie;
            else if (usage == 0x0c && usagePage == 0x09)
                cookies->gButtoncCookie = cookie;
            else if (usage == 0x39 && usagePage == 0x01)
                cookies->gHatCookie = cookie;

            // Unused?
            else if (usage == 0xbb && usagePage == 0x01)
                cookies->gU2Cookie = cookie;
            else if (usage == 0x05 && usagePage == 0x01)
                cookies->gU3Cookie = cookie;
            else if (usage == 0x01 && usagePage == 0x01)
                cookies->gU4Cookie = cookie;
        }
    } else {
        printf("copyMatchingElements failed with error %d\n", success);
    }

    return cookies;
}

int MT_HIDGamePad::getCookieValue(IOHIDElementCookie Cookie)
{
    HRESULT result;
    IOHIDEventStruct hidEvent;

    result = (*hidDeviceInterface)->getElementValue(hidDeviceInterface,
                                                    Cookie,&hidEvent);

    return hidEvent.value;

}

#endif // MT_GAMEPAD_MAC

void MT_HIDGamePad::Poll()
{
#ifdef MT_GAMEPAD_MAC
    // Check the queue for button presses
    CheckQueue();
    PollXAxis();
    PollYAxis();
    PollZAxis();
    PollWAxis();
#elif defined MT_GAMEPAD_USE_WX
    ButtonStates = myJoystick.GetButtonState();
    wxPoint XYpt = myJoystick.GetPosition();
    X = XYpt.x;
    Y = XYpt.y;
    Xf = (((float) X) - xcenter)*xnorm;
    Yf = (((float) Y) - ycenter)*ynorm;
    W = myJoystick.GetRudderPosition();
    Z = myJoystick.GetZPosition();
    Wf = (((float) W) - wcenter)*wnorm;
    Zf = (((float) Z) - zcenter)*znorm;

    // gamepadcontroller expects a number from -128 to 128
    X = (int) (128.0*Xf);
    Y = (int) (128.0*Yf);
    Z = (int) (128.0*Zf);
    W = (int) (128.0*Wf);

    //printf("%d, %d, %d, %d, %f, %f, %f, %f\n", X, Y, W, Z, Xf, Yf, Wf, Zf);

#endif  // MT_GAMEPAD_USE_WX

    PollHatState();
    PollU2State();
    PollU3State();
    PollU4State();
}

#ifdef MT_GAMEPAD_MAC
void MT_HIDGamePad::CheckQueue()
{
  
    if(!StatusGood)
    {
        return;
    }
  
    //HRESULT result;
    IOHIDEventStruct event;
    AbsoluteTime zeroTime = {0,0};
  
    while( (*ButtonQueue)->getNextEvent(ButtonQueue , &event, zeroTime, 0) != kIOReturnUnderrun )
    {
        if(event.elementCookie == gpCookies->gButton1Cookie){
            MT_SETBIT(ButtonStates, MT_BUTTON1, event.value);
            continue;
        }  
        if(event.elementCookie == gpCookies->gButton2Cookie){
            MT_SETBIT(ButtonStates, MT_BUTTON2, event.value);
            continue;
        } 
        if(event.elementCookie == gpCookies->gButton3Cookie){
            MT_SETBIT(ButtonStates, MT_BUTTON3, event.value);
            continue;
        } 
        if(event.elementCookie == gpCookies->gButton4Cookie){
            MT_SETBIT(ButtonStates, MT_BUTTON4, event.value);
            continue;
        } 
        if(event.elementCookie == gpCookies->gButton5Cookie){
            MT_SETBIT(ButtonStates, MT_BUTTON5, event.value);
            continue;
        } 
        if(event.elementCookie == gpCookies->gButton6Cookie){
            MT_SETBIT(ButtonStates, MT_BUTTON6, event.value);
            continue;
        } 
        if(event.elementCookie == gpCookies->gButton7Cookie){
            MT_SETBIT(ButtonStates, MT_BUTTON7, event.value);
            continue;
        } 
        if(event.elementCookie == gpCookies->gButton8Cookie){
            MT_SETBIT(ButtonStates, MT_BUTTON8, event.value);
            continue;
        } 
        if(event.elementCookie == gpCookies->gButton9Cookie){
            MT_SETBIT(ButtonStates, MT_BUTTON9, event.value);
            continue;
        } 
        if(event.elementCookie == gpCookies->gButtonaCookie){
            MT_SETBIT(ButtonStates, MT_BUTTONa, event.value);
            continue;
        } 
        if(event.elementCookie == gpCookies->gButtonbCookie){
            MT_SETBIT(ButtonStates, MT_BUTTONb, event.value);
            continue;
        } 
        if(event.elementCookie == gpCookies->gButtoncCookie){
            MT_SETBIT(ButtonStates, MT_BUTTONc, event.value);   
            continue;
        } 
    }
  
}
#endif

#ifdef MT_GAMEPAD_MAC
//////////////////////////////////////////////////////////////
//  Functions to poll the current value of a button state
//////////////////////////////////////////////////////////////
int MT_HIDGamePad::PollButton1State() { 
    MT_SETBIT(ButtonStates,MT_BUTTON1,getCookieValue(gpCookies->gButton1Cookie));
    return Button1State();
}

int MT_HIDGamePad::PollButton2State() {
    MT_SETBIT(ButtonStates,MT_BUTTON2,getCookieValue(gpCookies->gButton2Cookie));
    return Button2State();
}

int MT_HIDGamePad::PollButton3State() {
    MT_SETBIT(ButtonStates,MT_BUTTON3,getCookieValue(gpCookies->gButton3Cookie));
    return Button3State();
}

int MT_HIDGamePad::PollButton4State() {
    MT_SETBIT(ButtonStates,MT_BUTTON4,getCookieValue(gpCookies->gButton4Cookie));
    return Button4State();
}

int MT_HIDGamePad::PollButton5State() {
    MT_SETBIT(ButtonStates,MT_BUTTON5,getCookieValue(gpCookies->gButton5Cookie));
    return Button5State();
}

int MT_HIDGamePad::PollButton6State() {
    MT_SETBIT(ButtonStates,MT_BUTTON6,getCookieValue(gpCookies->gButton6Cookie));
    return Button6State();
}

int MT_HIDGamePad::PollButton7State() {
    MT_SETBIT(ButtonStates,MT_BUTTON7,getCookieValue(gpCookies->gButton7Cookie));
    return Button7State();
}

int MT_HIDGamePad::PollButton8State() {
    MT_SETBIT(ButtonStates,MT_BUTTON8,getCookieValue(gpCookies->gButton8Cookie));
    return Button8State();
}

int MT_HIDGamePad::PollButton9State() {
    MT_SETBIT(ButtonStates,MT_BUTTON9,getCookieValue(gpCookies->gButton9Cookie));
    return Button9State();
}

int MT_HIDGamePad::PollButtonaState() {
    MT_SETBIT(ButtonStates,MT_BUTTONa,getCookieValue(gpCookies->gButtonaCookie));
    return ButtonaState();
}

int MT_HIDGamePad::PollButtonbState() {
    MT_SETBIT(ButtonStates,MT_BUTTONb,getCookieValue(gpCookies->gButtonbCookie));
    return ButtonbState();
}

int MT_HIDGamePad::PollButtoncState() {
    MT_SETBIT(ButtonStates,MT_BUTTONc,getCookieValue(gpCookies->gButtoncCookie));
    return ButtoncState();
}

int MT_HIDGamePad::PollHatState() {
    Hat = getCookieValue(gpCookies->gHatCookie);
    return Hat;
}

int MT_HIDGamePad::PollU2State() {
    U2 = getCookieValue(gpCookies->gU2Cookie);
    return U2;
}

int MT_HIDGamePad::PollU3State() {
    U3 = getCookieValue(gpCookies->gU3Cookie);
    return U3;
}

int MT_HIDGamePad::PollU4State() {
    U4 = getCookieValue(gpCookies->gU4Cookie);
    return U4;
}

// Axes Polling
int MT_HIDGamePad::PollXAxis() 
{ 
    X = getCookieValue(gpCookies->gXAxisCookie); 
    Xf = (((float) X) - xcenter)*xnorm;
    return X; 
}

int MT_HIDGamePad::PollYAxis()
{ 
    Y = -getCookieValue(gpCookies->gYAxisCookie); 
    Yf = (((float) Y) - ycenter)*ynorm;
    return Y; 
} 
        
int MT_HIDGamePad::PollZAxis()
{ 
    // NOTE - this gets reported as a slider, from 0 to 255
    //   here we subtract 128 to make it act like the other axes
    Z = 128 - getCookieValue(gpCookies->gZAxisCookie);
    Zf = (((float) Z) - zcenter)*znorm;
    return Z; 
}

int MT_HIDGamePad::PollWAxis() 
{ 
    W = getCookieValue(gpCookies->gWAxisCookie);
    Wf = (((float) W) - wcenter)*wnorm;
    return W; 
}

#elif defined(MT_GAMEPAD_USE_WX)
int MT_HIDGamePad::PollButton1State()
{
    ButtonStates = myJoystick.GetButtonState();
    return Button1State();
}

int MT_HIDGamePad::PollButton2State()
{
    ButtonStates = myJoystick.GetButtonState();
    return Button2State();
}

int MT_HIDGamePad::PollButton3State()
{
    ButtonStates = myJoystick.GetButtonState();
    return Button3State();
}

int MT_HIDGamePad::PollButton4State()
{
    ButtonStates = myJoystick.GetButtonState();
    return Button4State();
}

int MT_HIDGamePad::PollButton5State()
{
    ButtonStates = myJoystick.GetButtonState();
    return Button5State();
}

int MT_HIDGamePad::PollButton6State()
{
    ButtonStates = myJoystick.GetButtonState();
    return Button6State();
}

int MT_HIDGamePad::PollButton7State()
{
    ButtonStates = myJoystick.GetButtonState();
    return Button7State();
}

int MT_HIDGamePad::PollButton8State()
{
    ButtonStates = myJoystick.GetButtonState();
    return Button8State();
}

int MT_HIDGamePad::PollButton9State()
{
    ButtonStates = myJoystick.GetButtonState();
    return Button9State();
}

int MT_HIDGamePad::PollButtonaState()
{
    ButtonStates = myJoystick.GetButtonState();
    return ButtonaState();
}

int MT_HIDGamePad::PollButtonbState()
{
    ButtonStates = myJoystick.GetButtonState();
    return ButtonbState();
}

int MT_HIDGamePad::PollButtoncState()
{
    ButtonStates = myJoystick.GetButtonState();
    return ButtoncState();
}

int MT_HIDGamePad::PollHatState()
{
    // TODO
    return 0;
}

int MT_HIDGamePad::PollU2State()
{
    // TODO
    return 0;
}

int MT_HIDGamePad::PollU3State()
{
    // TODO
    return 0;
}

int MT_HIDGamePad::PollU4State()
{
    // TODO
    return 0;
}

int MT_HIDGamePad::PollXAxis()
{
    wxPoint pt = myJoystick.GetPosition();
    X = pt.x;
    Xf = (((float) X) - xcenter)*xnorm;
    X = (int) (128.0*Xf);
    return X;
}

int MT_HIDGamePad::PollYAxis()
{
    wxPoint pt = myJoystick.GetPosition();
    Y = pt.y;
    Yf = (((float) Y) - ycenter)*ynorm;
    Y = (int) (128.0*Yf);
    return Y;
}

int MT_HIDGamePad::PollZAxis()
{
    Z = myJoystick.GetZPosition();
    Zf = (((float) Z) - zcenter)*znorm;
    Z = (int) (128.0*Zf);
    return Z;
}

int MT_HIDGamePad::PollWAxis()
{
    W = myJoystick.GetRudderPosition();
    Wf = (((float) W) - wcenter)*wnorm;
    W = (int) (128.0*Wf);
    return W;
}

#endif 
