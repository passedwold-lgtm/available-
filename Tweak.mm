//Library
#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#import <objc/runtime.h>
#import <AudioToolbox/AudioToolbox.h>
#import <os/log.h>
#import "pthread.h"
#include <math.h>
#include <deque>
#include <vector>
#include <fstream>

//Config
#import "Config.h"

//Lib
#include "Lib/Obfuscate.h"
#include "Lib/Memory.h"

//UI
#import "UI/UIManager.h"
#import "UI/TypeSelectionView.h"
#include "Game/GameFunction.h"

@interface UIManager (HackLoop)
- (void)onTimer;
@end

@implementation UIManager (HackLoop)
- (void)onTimer {
    render_lop();
}
@end

static UIManager *_UIManager = [UIManager shared];

static void SetupUI() {
    [_UIManager setupUI];

    MenuView *menu = _UIManager.menu;
    menu.telegramURL = @"https://t.me/ioshacktutorial";
    menu.discordURL = @"https://discord.gg/iostutorials";
    
    [menu setMenuTitle:@"MENU NATIVE IOS"];
    [menu setMenuSubtitle:@"Native Menu Objective-C"];
    [menu canMove:YES];

    [menu addTab:@[@"ESP", @"AIMBOT", @"MEMORY", @"SETTINGS"]];

    // TAB 0: ESP
    [menu setTabIndex:0];
    [menu addSectionTitle:@"ESP"];
    

    //ESP_Box
    [menu addFeatureSwitch:@"ESP Box" description:@"Hiển thị box ESP." handler:^(BOOL isOn) {
        Vars.ESP_Box = isOn;
    }];

    //ESP_Name
    [menu addFeatureSwitch:@"ESP Name" description:@"Hiển thị name ESP." handler:^(BOOL isOn) {
        Vars.ESP_Name = isOn;
    }];
    
    //ESP_Distance
    [menu addFeatureSwitch:@"ESP Distance" description:@"Hiển thị distance ESP." handler:^(BOOL isOn) {
        Vars.ESP_Distance = isOn;
    }];

    //ESP_MaxDistance
    [menu addSlider:@"ESP Max Distance" max:100.0 min:1.0 value:Vars.ESP_MaxDistance handler:^(CGFloat value) {
        Vars.ESP_MaxDistance = value;
    }];

    // TAB 1: AIMBOT
    [menu setTabIndex:1];
    [menu addSectionTitle:@"AIMBOT"];

    //AIMBOT_Aimbot
    [menu addFeatureSwitch:@"Aimbot" description:@"Hiển thị aimbot." handler:^(BOOL isOn) {
        Vars.AIMBOT_Aimbot = isOn;
    }];

    //AIMBOT_AimbotKey
    [menu addFeatureSwitch:@"Aimbot Key" description:@"Hiển thị aimbot key." handler:^(BOOL isOn) {
        Vars.AIMBOT_AimbotKey = isOn;
    }];

    //AIMBOT_AimbotKeyCode
    [menu addSlider:@"Aimbot Key Code" max:100.0 min:0.0 value:Vars.AIMBOT_AimbotKeyCode handler:^(CGFloat value) {
        Vars.AIMBOT_AimbotKeyCode = value;
    }];
    
    //demo button
    [menu addButton:@"Demo Button" withHandler:^{
        NSLog(@"Demo Button Pressed");
    }];

    //demo text field
    [menu addTextField:@"Demo Text Field" placeholder:@"Demo Text Field" handler:^(NSString *text) {
        NSLog(@"Demo Text Field: %@", text);
    }];

    //demo combo selector
    [menu addComboSelector:@"Demo Combo Selector" options:@[@"Option 1", @"Option 2", @"Option 3"] selectedIndex:0 handler:^(NSInteger index) {
        NSLog(@"Demo Combo Selector: %ld", index);
    }];

    // TAB 2: MEMORY
    [menu setTabIndex:2];
    [menu addSectionTitle:@"MEMORY"];

    //MEMORY_Memory
    [menu addFeatureSwitch:@"Memory" description:@"Hiển thị memory." handler:^(BOOL isOn) {
        Vars.MEMORY_Memory = isOn;
    }];

    //MEMORY_MemoryKey
    [menu addFeatureSwitch:@"Memory Key" description:@"Hiển thị memory key." handler:^(BOOL isOn) {
        Vars.MEMORY_MemoryKey = isOn;
    }];

    //MEMORY_MemoryKeyCode
    [menu addSlider:@"Memory Key Code" max:100.0 min:0.0 value:Vars.MEMORY_MemoryKeyCode handler:^(CGFloat value) {
        Vars.MEMORY_MemoryKeyCode = value;
    }];

    //demo button
    [menu addButton:@"Demo Button" withHandler:^{
        NSLog(@"Demo Button Pressed");
    }];

    //demo text field
    [menu addTextField:@"Demo Text Field" placeholder:@"Demo Text Field" handler:^(NSString *text) {
        NSLog(@"Demo Text Field: %@", text);
    }];


    // TAB 3: SETTINGS
    [menu setTabIndex:3];
    [menu addSectionTitle:@"SETTINGS"];
    
    [menu addComboSelector:@"Màu chủ đạo" options:@[@"Xanh dương", @"Đỏ rực", @"Xanh lá", @"Vàng", @"Tím", @"Hồng"] selectedIndex:0 handler:^(NSInteger index) {
        UIColor *selectedColor;
        switch (index) {
            case 0: selectedColor = [UIColor colorWithRed:110.0/255.0 green:142.0/255.0 blue:251.0/255.0 alpha:1.0]; break;
            case 1: selectedColor = [UIColor colorWithRed:255.0/255.0 green:59.0/255.0 blue:48.0/255.0 alpha:1.0]; break;
            case 2: selectedColor = [UIColor colorWithRed:52.0/255.0 green:199.0/255.0 blue:89.0/255.0 alpha:1.0]; break;
            case 3: selectedColor = [UIColor colorWithRed:255.0/255.0 green:204.0/255.0 blue:0.0/255.0 alpha:1.0]; break;
            case 4: selectedColor = [UIColor colorWithRed:175.0/255.0 green:82.0/255.0 blue:222.0/255.0 alpha:1.0]; break;
            case 5: selectedColor = [UIColor colorWithRed:255.0/255.0 green:45.0/255.0 blue:85.0/255.0 alpha:1.0]; break;
            default: selectedColor = [UIColor colorWithRed:110.0/255.0 green:142.0/255.0 blue:251.0/255.0 alpha:1.0]; break;
        }
        [menu setMenuAccentColor:selectedColor];
    }];

    [menu addThemeSlider:@"Độ trong suốt" property:@"opacity" max:1.0 min:0.2 value:1.0 handler:nil];
    [menu addThemeSlider:@"Độ bo góc" property:@"corner" max:30.0 min:0.0 value:24.0 handler:nil];
    [menu addThemeSlider:@"Độ dày viền" property:@"border" max:5.0 min:0.0 value:1.0 handler:nil];
    
    [menu addSectionTitle:@"HÀNH ĐỘNG"];
    [menu addButton:@"Reset Cài đặt" withHandler:^{
        [menu setMenuCornerRadius:15.0];
        [menu setMenuBorderWidth:1.0];
        menu.alpha = 1.0;
    }];
    
    [menu updateLayout];
}

// static void initializeSomething(void) {
// }

static void Initialize() {
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(WAIT_TIME * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        // initializeSomething();
        game_sdk->init();
        CADisplayLink *displayLink = [CADisplayLink displayLinkWithTarget:_UIManager selector:@selector(onTimer)];
        [displayLink addToRunLoop:[NSRunLoop mainRunLoop] forMode:NSDefaultRunLoopMode];
        SetupUI(); 
    });
}

static void didFinishLaunching(CFNotificationCenterRef center, void *observer, CFStringRef name, const void *object, CFDictionaryRef info) {
    Initialize();
}

static void launchEvent(void) {
    CFNotificationCenterAddObserver(CFNotificationCenterGetLocalCenter(), NULL, &didFinishLaunching, (CFStringRef)UIApplicationDidFinishLaunchingNotification, NULL, CFNotificationSuspensionBehaviorDrop);
}

__attribute__((constructor)) static void initialize(void) { 
    launchEvent();
}
