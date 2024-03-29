#include <list>
#include <vector>
#include <string.h>
#include <pthread.h>
#include <cstring>
#include <jni.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <dlfcn.h>
#include "Includes/Logger.h"
#include "Includes/obfuscate.h"
#include "Includes/Utils.h"

#include "KittyMemory/MemoryPatch.h"
#include "Menu.h"

//Target lib here
#define targetLibName OBFUSCATE("libFileA.so")

#include "Includes/Macros.h"

// fancy struct for patches for kittyMemory
struct My_Patches {
    // let's assume we have patches for these functions for whatever game
    // like show in miniMap boolean function
    MemoryPatch GodMode, GodMode2, SliderExample;
    // etc...
} hexPatches;

bool feature1, feature2, featureHookToggle, Health;
int sliderValue = 1, level = 0;
void *instanceBtn;

// Hooking examples. Assuming you know how to write hook
void (*AddMoneyExample)(void *instance, int amount);

bool (*old_get_BoolExample)(void *instance);
bool get_BoolExample(void *instance) {
    if (instance != NULL && featureHookToggle) {
        return true;
    }
    return old_get_BoolExample(instance);
}

float (*old_get_FloatExample)(void *instance);
float get_FloatExample(void *instance) {
    if (instance != NULL && sliderValue > 1) {
        return (float) sliderValue;
    }
    return old_get_FloatExample(instance);
}

int (*old_Level)(void *instance);
int Level(void *instance) {
    if (instance != NULL && level) {
        return (int) level;
    }
    return old_Level(instance);
}

void (*old_FunctionExample)(void *instance);
void FunctionExample(void *instance) {
    instanceBtn = instance;
    if (instance != NULL) {
        if (Health) {
            *(int *) ((uint64_t) instance + 0x48) = 999;
        }
    }
    return old_FunctionExample(instance);
}

// we will run our hacks in a new thread so our while loop doesn't block process main thread
void *hack_thread(void *) {
    LOGI(OBFUSCATE("pthread created"));

    //Check if target lib is loaded
    do {
        sleep(1);
    } while (!isLibraryLoaded(targetLibName));

    //Anti-lib rename
    /*
    do {
        sleep(1);
    } while (!isLibraryLoaded("libYOURNAME.so"));*/

    LOGI(OBFUSCATE("%s has been loaded"), (const char *) targetLibName);

#if defined(__aarch64__) //To compile this code for arm64 lib only. Do not worry about greyed out highlighting code, it still works
    // New way to patch hex via KittyMemory without need to  specify len. Spaces or without spaces are fine
    // ARM64 assembly example
    // MOV X0, #0x0 = 00 00 80 D2
    // RET = C0 03 5F D6
    hexPatches.GodMode = MemoryPatch::createWithHex(targetLibName,
                                                    string2Offset(OBFUSCATE("0x123456")),
                                                    OBFUSCATE("00 00 80 D2 C0 03 5F D6"));
    //You can also specify target lib like this
    hexPatches.GodMode2 = MemoryPatch::createWithHex("libtargetLibHere.so",
                                                     string2Offset(OBFUSCATE("0x222222")),
                                                     OBFUSCATE("20 00 80 D2 C0 03 5F D6"));

    // Hook example. Comment out if you don't use hook
    // Strings in macros are automatically obfuscated. No need to obfuscate!
    HOOK("str", FunctionExample, old_FunctionExample);
    HOOK_LIB("libFileB.so", "0x123456", FunctionExample, old_FunctionExample);
    HOOK_NO_ORIG("0x123456", FunctionExample);
    HOOK_LIB_NO_ORIG("libFileC.so", "0x123456", FunctionExample);
    HOOKSYM("__SymbolNameExample", FunctionExample, old_FunctionExample);
    HOOKSYM_LIB("libFileB.so", "__SymbolNameExample", FunctionExample, old_FunctionExample);
    HOOKSYM_NO_ORIG("__SymbolNameExample", FunctionExample);
    HOOKSYM_LIB_NO_ORIG("libFileB.so", "__SymbolNameExample", FunctionExample);

    // Patching offsets directly. Strings are automatically obfuscated too!
    PATCHOFFSET("0x20D3A8", "00 00 A0 E3 1E FF 2F E1");
    PATCHOFFSET_LIB("libFileB.so", "0x20D3A8", "00 00 A0 E3 1E FF 2F E1");

    AddMoneyExample = (void(*)(void *,int))getAbsoluteAddress(targetLibName, 0x123456);

#else //To compile this code for armv7 lib only.
    // New way to patch hex via KittyMemory without need to specify len. Spaces or without spaces are fine
    // ARMv7 assembly example
    // MOV R0, #0x0 = 00 00 A0 E3
    // BX LR = 1E FF 2F E1
    hexPatches.GodMode = MemoryPatch::createWithHex(targetLibName, //Normal obfuscate
                                                    string2Offset(OBFUSCATE("0x123456")),
                                                    OBFUSCATE("00 00 A0 E3 1E FF 2F E1"));
    //You can also specify target lib like this
    hexPatches.GodMode2 = MemoryPatch::createWithHex("libtargetLibHere.so",
                                                     string2Offset(OBFUSCATE("0x222222")),
                                                     OBFUSCATE("01 00 A0 E3 1E FF 2F E1"));

    // Hook example. Comment out if you don't use hook
    // Strings in macros are automatically obfuscated. No need to obfuscate!
    HOOK("str", FunctionExample, old_FunctionExample);
    HOOK_LIB("libFileB.so", "0x123456", FunctionExample, old_FunctionExample);
    HOOK_NO_ORIG("0x123456", FunctionExample);
    HOOK_LIB_NO_ORIG("libFileC.so", "0x123456", FunctionExample);
    HOOKSYM("__SymbolNameExample", FunctionExample, old_FunctionExample);
    HOOKSYM_LIB("libFileB.so", "__SymbolNameExample", FunctionExample, old_FunctionExample);
    HOOKSYM_NO_ORIG("__SymbolNameExample", FunctionExample);
    HOOKSYM_LIB_NO_ORIG("libFileB.so", "__SymbolNameExample", FunctionExample);

    // Patching offsets directly. Strings are automatically obfuscated too!
    PATCHOFFSET("0x20D3A8", "00 00 A0 E3 1E FF 2F E1");
    PATCHOFFSET_LIB("libFileB.so", "0x20D3A8", "00 00 A0 E3 1E FF 2F E1");

    AddMoneyExample = (void (*)(void *, int)) getAbsoluteAddress(targetLibName, 0x123456);

    LOGI(OBFUSCATE("Done"));
#endif

    return NULL;
}

//JNI calls
extern "C" {

// Do not change or translate the first text unless you know what you are doing
// Assigning feature numbers is optional. Without it, it will automatically count for you, starting from 0
// Assigned feature numbers can be like any numbers 1,3,200,10... instead in order 0,1,2,3,4,5...
// ButtonLink, Category, RichTextView and RichWebView is not counted. They can't have feature number assigned
// Toggle, ButtonOnOff and Checkbox can be switched on by default, if you add True_. Example: CheckBox_True_The Check Box
// To learn HTML, go to this page: https://www.w3schools.com/

JNIEXPORT jobjectArray
JNICALL
Java_uk_lgl_modmenu_FloatingModMenuService_getFeatureList(JNIEnv *env, jobject context) {
    jobjectArray ret;

    //Toasts added here so it's harder to remove it
    MakeToast(env, context, OBFUSCATE("Modded by ImTienNguyenZ"), Toast::LENGTH_LONG);

    const char *features[] = {
            OBFUSCATE("Category_Tap Menu 1"), //Not counted
            OBFUSCATE("Toggle_The toggle"),
            OBFUSCATE(
                    "100_Toggle_True_The toggle 2"), //This one have feature number assigned, and switched on by default
            OBFUSCATE("110_Toggle_The toggle 3"), //This one too
            OBFUSCATE("SeekBar_The slider_1_100"),
            OBFUSCATE("SeekBar_Kittymemory slider example_1_5"),
            OBFUSCATE("Spinner_The spinner_Items 1,Items 2,Items 3"),
            OBFUSCATE("Button_The button"),
            OBFUSCATE("ButtonLink_The button with link_https://www.youtube.com/"), //Not counted
            OBFUSCATE("ButtonOnOff_The On/Off button"),
            OBFUSCATE("CheckBox_The Check Box"),
            OBFUSCATE("InputValue_Input number"),
            OBFUSCATE("InputValue_1000_Input number 2"), //Max value
            OBFUSCATE("InputText_Input text"),
            OBFUSCATE("RadioButton_Radio buttons_OFF,Mod 1,Mod 2,Mod 3"),

            //Create new collapse
            OBFUSCATE("Collapse_Collapse 1"),
            OBFUSCATE("CollapseAdd_Toggle_The toggle"),
            OBFUSCATE("CollapseAdd_Toggle_The toggle"),
            OBFUSCATE("123_CollapseAdd_Toggle_The toggle"),
            OBFUSCATE("CollapseAdd_Button_The button"),

            //Create new collapse again
            OBFUSCATE("Collapse_Collapse 2"),
            OBFUSCATE("CollapseAdd_SeekBar_The slider_1_100"),
            OBFUSCATE("CollapseAdd_InputValue_Input number"),

            OBFUSCATE("RichTextView_This is text view, not fully HTML."
                      "<b>Bold</b> <i>italic</i> <u>underline</u>"
                      "<br />New line <font color='red'>Support colors</font>"
                      "<br/><big>bigger Text</big>"),
            OBFUSCATE("RichWebView_<html><head><style>body{color: white;}</style></head><body>"
                      "This is WebView, with REAL HTML support!"
                      "<div style=\"background-color: darkblue; text-align: center;\">Support CSS</div>"
                      "<marquee style=\"color: green; font-weight:bold;\" direction=\"left\" scrollamount=\"5\" behavior=\"scroll\">This is <u>scrollable</u> text</marquee>"
                      "</body></html>"),
                      
                      
                      
                      
                      
                      
            "Category2_Tap Menu 2", //Not counted
            "Toggle2_The toggle",
            "Toggle2_The toggle 2", //This one have feature number assigned, and switched on by default
            "Toggle2_The toggle 3", //This one too
            "SeekBar2_The Slider Value_0_100",
            "SeekBar2_The Slider Value2_0_300",
            "SeekBar2_Kittymemory slider example_0_5",
            "Spinner2_The spinner_Items 1,Items 2,Items 3",
            "ButtonLink2_The button link Your Channel_https://www.youtube.com/", //Not counted
            "ButtonOnOff2_The On/Off button",
            "CheckBox2_The Check Box",
            "InputValue2_Input number",
            "InputText2_Input text",
            "RadioButton2_Radio buttons_OFF,Mod 1,Mod 2,Mod 3",

            //Create new collapse
            "Collapse2_Collapse 1",
            "CollapseAdd2_Toggle2_The toggle",
            "CollapseAdd2_Toggle2_The toggle",
            "123_CollapseAdd2_Toggle2_The toggle",
            "CollapseAdd2_Button2_The button",

            //Create new collapse again
            "Collapse2_Collapse 2",
            "CollapseAdd2_SeekBar2_The slider_1_100",
            "CollapseAdd2_InputValue2_Input number",

            "RichTextView2_This is text view, not fully HTML."
                      "<b>Bold</b> <i>italic</i> <u>underline</u>"
                      "<br />New line <font color='red'>Support colors</font>"
                      "<br/><big>bigger Text</big>",
                      
                      
            "RichWebView2_<html><head><style>body{color: white;}</style></head><body>"
                      "This is WebView, with REAL HTML support!"
                      "<div style=\"background-color: darkblue; text-align: center;\">Support CSS</div>"
                      "<marquee style=\"color: green; font-weight:bold;\" direction=\"left\" scrollamount=\"5\" behavior=\"scroll\">This is <u>scrollable</u> text</marquee>"
                      "</body></html>",       
                      
                      
                      
                      
                      
                      
            "Category3_Tap Menu 3", //Not counted
            "Toggle3_The toggle",
            "Toggle3_The toggle 2", //This one have feature number assigned, and switched on by default
            "Toggle3_The toggle 3", //This one too
            "SeekBar3_The Slider Value_0_100",
            "SeekBar3_The Slider Value2_0_300",
            "SeekBar3_Kittymemory slider example_0_5",
            "Spinner3_The spinner_Items 1,Items 2,Items 3",
            "ButtonLink3_The button link Your Channel_https://www.youtube.com/", //Not counted
            "ButtonOnOff3_The On/Off button",
            "CheckBox3_The Check Box",
            "InputValue3_Input number",
            "InputText3_Input text",
            "RadioButton3_Radio buttons_OFF,Mod 1,Mod 2,Mod 3",

            //Create new collapse
            "Collapse3_Collapse 1",
            "CollapseAdd3_Toggle3_The toggle",
            "CollapseAdd3_Toggle3_The toggle",
            "123_CollapseAdd3_Toggle3_The toggle",
            "CollapseAdd3_Button3_The button",

            //Create new collapse again
            "Collapse3_Collapse 2",
            "CollapseAdd3_SeekBar3_The slider_1_100",
            "CollapseAdd3_InputValue3_Input number",

            "RichTextView3_This is text view, not fully HTML."
                      "<b>Bold</b> <i>italic</i> <u>underline</u>"
                      "<br />New line <font color='red'>Support colors</font>"
                      "<br/><big>bigger Text</big>",
                      
                      
            "RichWebView3_<html><head><style>body{color: white;}</style></head><body>"
                      "This is WebView, with REAL HTML support!"
                      "<div style=\"background-color: darkblue; text-align: center;\">Support CSS</div>"
                      "<marquee style=\"color: green; font-weight:bold;\" direction=\"left\" scrollamount=\"5\" behavior=\"scroll\">This is <u>scrollable</u> text</marquee>"
                      "</body></html>", 
                      
                      
                      
                      
                      
                      
            "Category4_Tap Menu 4", //Not counted
            "Toggle4_The toggle",
            "Toggle4_The toggle 2", //This one have feature number assigned, and switched on by default
            "Toggle4_The toggle 3", //This one too
            "SeekBar4_The Slider Value_0_100",
            "SeekBar4_The Slider Value2_0_300",
            "SeekBar4_Kittymemory slider example_0_5",
            "Spinner4_The spinner_Items 1,Items 2,Items 3",
            "ButtonLink4_The button link Your Channel_https://www.youtube.com/", //Not counted
            "ButtonOnOff4_The On/Off button",
            "CheckBox4_The Check Box",
            "InputValue4_Input number",
            "InputText4_Input text",
            "RadioButton4_Radio buttons_OFF,Mod 1,Mod 2,Mod 3",

            //Create new collapse
            "Collapse4_Collapse 1",
            "CollapseAdd4_Toggle4_The toggle",
            "CollapseAdd4_Toggle4_The toggle",
            "123_CollapseAdd4_Toggle4_The toggle",
            "CollapseAdd4_Button4_The button",

            //Create new collapse again
            "Collapse4_Collapse 2",
            "CollapseAdd4_SeekBar4_The slider_1_100",
            "CollapseAdd4_InputValue4_Input number",

            "RichTextView4_This is text view, not fully HTML."
                      "<b>Bold</b> <i>italic</i> <u>underline</u>"
                      "<br />New line <font color='red'>Support colors</font>"
                      "<br/><big>bigger Text</big>",
                      
                      
            "RichWebView4_<html><head><style>body{color: white;}</style></head><body>"
                      "This is WebView, with REAL HTML support!"
                      "<div style=\"background-color: darkblue; text-align: center;\">Support CSS</div>"
                      "<marquee style=\"color: green; font-weight:bold;\" direction=\"left\" scrollamount=\"5\" behavior=\"scroll\">This is <u>scrollable</u> text</marquee>"
                      "</body></html>",
                      
                      
                      
                      
                      
                      
            "Category5_Tap Menu 5", //Not counted
            "Toggle5_The toggle",
            "Toggle5_The toggle 2", //This one have feature number assigned, and switched on by default
            "Toggle5_The toggle 3", //This one too
            "SeekBar5_The Slider Value_0_100",
            "SeekBar5_The Slider Value2_0_300",
            "SeekBar5_Kittymemory slider example_0_5",
            "Spinner5_The spinner_Items 1,Items 2,Items 3",
            "ButtonLink5_The button link Your Channel_https://www.youtube.com/", //Not counted
            "ButtonOnOff5_The On/Off button",
            "CheckBox5_The Check Box",
            "InputValue5_Input number",
            "InputText5_Input text",
            "RadioButton5_Radio buttons_OFF,Mod 1,Mod 2,Mod 3",

            //Create new collapse
            "Collapse5_Collapse 1",
            "CollapseAdd5_Toggle5_The toggle",
            "CollapseAdd5_Toggle5_The toggle",
            "123_CollapseAdd5_Toggle5_The toggle",
            "CollapseAdd5_Button5_The button",

            //Create new collapse again
            "Collapse5_Collapse 2",
            "CollapseAdd5_SeekBar5_The slider_1_100",
            "CollapseAdd5_InputValue5_Input number",

            "RichTextView5_This is text view, not fully HTML."
                      "<b>Bold</b> <i>italic</i> <u>underline</u>"
                      "<br />New line <font color='red'>Support colors</font>"
                      "<br/><big>bigger Text</big>",
                      
                      
            "RichWebView5_<html><head><style>body{color: white;}</style></head><body>"
                      "This is WebView, with REAL HTML support!"
                      "<div style=\"background-color: darkblue; text-align: center;\">Support CSS</div>"
                      "<marquee style=\"color: green; font-weight:bold;\" direction=\"left\" scrollamount=\"5\" behavior=\"scroll\">This is <u>scrollable</u> text</marquee>"
                      "</body></html>"  ,
            
            
            "Category6_Tap Menu 6", //Not counted
            "Toggle6_The toggle",
            "Toggle6_The toggle 6", //This one have feature number assigned, and switched on by default
            "Toggle6_The toggle 3", //This one too
            "SeekBar6_The Slider Value_0_100",
            "SeekBar6_The Slider Value6_0_300",
            "SeekBar6_Kittymemory slider example_0_5",
            "Spinner6_The spinner_Items 1,Items 6,Items 3",
            "ButtonLink6_The button link Your Channel_https://www.youtube.com/", //Not counted
            "ButtonOnOff6_The On/Off button",
            "CheckBox6_The Check Box",
            "InputValue6_Input number",
            "InputText6_Input text",
            "RadioButton6_Radio buttons_OFF,Mod 1,Mod 6,Mod 3",

            //Create new collapse
            "Collapse6_Collapse 1",
            "CollapseAdd6_Toggle6_The toggle",
            "CollapseAdd6_Toggle6_The toggle",
            "123_CollapseAdd6_Toggle6_The toggle",
            "CollapseAdd6_Button6_The button",

            //Create new collapse again
            "Collapse6_Collapse 6",
            "CollapseAdd6_SeekBar6_The slider_1_100",
            "CollapseAdd6_InputValue6_Input number",

            "RichTextView6_This is text view, not fully HTML."
                      "<b>Bold</b> <i>italic</i> <u>underline</u>"
                      "<br />New line <font color='red'>Support colors</font>"
                      "<br/><big>bigger Text</big>",
                      
                      
            "RichWebView6_<html><head><style>body{color: white;}</style></head><body>"
                      "This is WebView, with REAL HTML support!"
                      "<div style=\"background-color: darkblue; text-align: center;\">Support CSS</div>"
                      "<marquee style=\"color: green; font-weight:bold;\" direction=\"left\" scrollamount=\"5\" behavior=\"scroll\">This is <u>scrollable</u> text</marquee>"
                      "</body></html>"
    };

    //Now you dont have to manually update the number everytime;
    int Total_Feature = (sizeof features / sizeof features[0]);
    ret = (jobjectArray)
            env->NewObjectArray(Total_Feature, env->FindClass(OBFUSCATE("java/lang/String")),
                                env->NewStringUTF(""));

    for (int i = 0; i < Total_Feature; i++)
        env->SetObjectArrayElement(ret, i, env->NewStringUTF(features[i]));

    pthread_t ptid;
    pthread_create(&ptid, NULL, antiLeech, NULL);

    return (ret);
}

JNIEXPORT void JNICALL
Java_uk_lgl_modmenu_Preferences_Changes(JNIEnv *env, jclass clazz, jobject obj,
                                        jint featNum, jstring featName, jint value,
                                        jboolean boolean, jstring str) {

    LOGD(OBFUSCATE("Feature name: %d - %s | Value: = %d | Bool: = %d | Text: = %s"), featNum,
         env->GetStringUTFChars(featName, 0), value,
         boolean, str != NULL ? env->GetStringUTFChars(str, 0) : "");

    //BE CAREFUL NOT TO ACCIDENTLY REMOVE break;

    switch (featNum) {
        case 0:
            feature2 = boolean;
            if (feature2) {
                // To print bytes you can do this
                //if (hexPatches.GodMode.Modify()) {
                //    LOGD(OBFUSCATE("Current Bytes: %s"),
                //         hexPatches.GodMode.get_CurrBytes().c_str());
                //}
                hexPatches.GodMode.Modify();
                hexPatches.GodMode2.Modify();
                //LOGI(OBFUSCATE("On"));
            } else {
                hexPatches.GodMode.Restore();
                hexPatches.GodMode2.Restore();
                //LOGI(OBFUSCATE("Off"));
            }
            break;
        case 100:
            break;
        case 110:
            break;
        case 1:
            if (value >= 1) {
                sliderValue = value;
            }
            break;
        case 2:
            switch (value) {
                //For noobies
                case 0:
                    hexPatches.SliderExample = MemoryPatch::createWithHex(
                            targetLibName, string2Offset(
                                    OBFUSCATE("0x100000")),
                            OBFUSCATE(
                                    "00 00 A0 E3 1E FF 2F E1"));
                    hexPatches.SliderExample.Modify();
                    break;
                case 1:
                    hexPatches.SliderExample = MemoryPatch::createWithHex(
                            targetLibName, string2Offset(
                                    OBFUSCATE("0x100000")),
                            OBFUSCATE(
                                    "01 00 A0 E3 1E FF 2F E1"));
                    hexPatches.SliderExample.Modify();
                    break;
                case 2:
                    hexPatches.SliderExample = MemoryPatch::createWithHex(
                            targetLibName,
                            string2Offset(
                                    OBFUSCATE("0x100000")),
                            OBFUSCATE(
                                    "02 00 A0 E3 1E FF 2F E1"));
                    hexPatches.SliderExample.Modify();
                    break;
            }
            break;
        case 3:
            switch (value) {
                case 0:
                    LOGD(OBFUSCATE("Selected item 1"));
                    break;
                case 1:
                    LOGD(OBFUSCATE("Selected item 2"));
                    break;
                case 2:
                    LOGD(OBFUSCATE("Selected item 3"));
                    break;
            }
            break;
        case 4:
            // Since we have instanceBtn as a field, we can call it out of Update hook function
            if (instanceBtn != NULL)
                AddMoneyExample(instanceBtn, 999999);
            // MakeToast(env, obj, OBFUSCATE("Button pressed"), Toast::LENGTH_SHORT);
            break;
        case 5:
            break;
        case 6:
            featureHookToggle = boolean;
            break;
        case 7:
            level = value;
            break;
        case 8:
            //MakeToast(env, obj, TextInput, Toast::LENGTH_SHORT);
            break;
        case 9:
            break;
    }
}
}

//No need to use JNI_OnLoad, since we don't use JNIEnv
//We do this to hide OnLoad from disassembler
__attribute__((constructor))
void lib_main() {
    // Create a new thread so it does not block the main thread, means the game would not freeze
    pthread_t ptid;
    pthread_create(&ptid, NULL, hack_thread, NULL);
}

/*
JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *globalEnv;
    vm->GetEnv((void **) &globalEnv, JNI_VERSION_1_6);
    return JNI_VERSION_1_6;
}
 */
