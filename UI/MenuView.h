@interface MenuView : UIView <UITextFieldDelegate, UIGestureRecognizerDelegate, UIScrollViewDelegate>

@property (nonatomic, assign) CGPoint lastLocation;
@property (nonatomic, strong) NSMutableDictionary *switches;
@property (nonatomic, strong) NSMutableDictionary *sliders;
@property (nonatomic, strong) NSMutableDictionary *sliderLabels;
@property (nonatomic, strong) NSMutableDictionary *buttons;
@property (nonatomic, strong) NSMutableDictionary *textFields;
@property (nonatomic, strong) UIColor *accentColor;
@property (nonatomic, strong) UILabel *titleLabel;
@property (nonatomic, strong) UILabel *subtitleLabel;
@property (nonatomic, strong) UIScrollView *scrollView;
@property (nonatomic, strong) UIView *contentView;
@property (nonatomic, strong) UIVisualEffectView *blurEffectView;
@property (nonatomic, strong) UISegmentedControl *tabBar;
@property (nonatomic, strong) UIView *tabSidebar;
@property (nonatomic, strong) UIScrollView *tabScrollView;
@property (nonatomic, strong) UIView *tabContainerView;
@property (nonatomic, strong) NSMutableArray *tabButtons;
@property (nonatomic, assign) NSInteger selectedTabIndex;
@property (nonatomic, assign) NSInteger currentCategoryCounter;
@property (nonatomic, strong) UIButton *telegramButton;
@property (nonatomic, strong) UIButton *discordButton;
@property (nonatomic, strong) UIButton *closeButton;
@property (nonatomic, assign) BOOL canMove;
@property (nonatomic, strong) UIPanGestureRecognizer *panGesture;

+ (instancetype)menuWithFrame:(CGRect)frame;
- (void)makeDraggable;
- (void)canMove:(BOOL)enabled;
- (void)addFeatureSwitch:(NSString *)title description:(NSString *)desc handler:(void (^)(BOOL isOn))handler;
- (void)addSlider:(NSString *)title max:(CGFloat)max min:(CGFloat)min value:(CGFloat)value handler:(void (^)(CGFloat value))handler;
- (void)addButton:(NSString *)title withHandler:(void (^)(void))handler;
- (void)addComboSelector:(NSString *)title options:(NSArray *)options selectedIndex:(NSInteger)index handler:(void (^)(NSInteger selectedIndex))handler;
- (void)addTextField:(NSString *)title placeholder:(NSString *)placeholder handler:(void (^)(NSString *text))handler;
@property (nonatomic, strong) NSString *telegramURL;
@property (nonatomic, strong) NSString *discordURL;

- (void)addSectionTitle:(NSString *)title;
- (void)setTabIndex:(NSInteger)index;
- (void)addTab:(NSArray<NSString *> *)tabNames;
- (void)addThemeSlider:(NSString *)title property:(NSString *)prop max:(CGFloat)max min:(CGFloat)min value:(CGFloat)value handler:(void (^)(CGFloat value))handler;
- (void)updateLayout;
- (void)setMenuAccentColor:(UIColor *)color;
- (void)setMenuGlassEffect:(BOOL)enabled;
- (void)setMenuCornerRadius:(CGFloat)radius;
- (void)setMenuBorderWidth:(CGFloat)width;
- (void)setMenuTitle:(NSString *)title;
- (void)setMenuSubtitle:(NSString *)subtitle;
- (void)setFooterText:(NSString *)text;
@end

@implementation MenuView

+ (instancetype)menuWithFrame:(CGRect)frame {
    MenuView *menu = [[MenuView alloc] initWithFrame:frame];
    [menu setup];
    return menu;
}

- (void)setup {
    self.accentColor = [UIColor colorWithRed:110.0/255.0 green:142.0/255.0 blue:251.0/255.0 alpha:1.0];
    self.layer.cornerRadius = 15.0; // Menu mặc định bo góc 15.0
    self.layer.masksToBounds = YES;
    self.layer.borderWidth = 1.0;
    self.layer.borderColor = [UIColor colorWithWhite:1.0 alpha:0.1].CGColor;
    self.alpha = 0.65; // Default opacity 65%
    self.currentCategoryCounter = 0;
    self.selectedTabIndex = 0;
    
    UIBlurEffect *blurEffect = [UIBlurEffect effectWithStyle:UIBlurEffectStyleDark];
    self.blurEffectView = [[UIVisualEffectView alloc] initWithEffect:blurEffect];
    self.blurEffectView.frame = self.bounds;
    self.blurEffectView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    [self addSubview:self.blurEffectView];
    
    UIView *gradientOverlay = [[UIView alloc] initWithFrame:self.bounds];
    gradientOverlay.backgroundColor = [UIColor colorWithRed:15.0/255.0 green:15.0/255.0 blue:25.0/255.0 alpha:0.4];
    [self addSubview:gradientOverlay];
    
    // Sidebar width for tabs
    CGFloat sidebarWidth = 110;
    CGFloat headerHeight = 50;
    CGFloat contentStartX = sidebarWidth;
    CGFloat contentWidth = self.frame.size.width - sidebarWidth;
    
    // Left Sidebar for Tabs with ScrollView
    self.tabSidebar = [[UIView alloc] initWithFrame:CGRectMake(0, headerHeight, sidebarWidth, self.frame.size.height - headerHeight)];
    self.tabSidebar.backgroundColor = [UIColor colorWithWhite:0.0 alpha:0.2];
    [self addSubview:self.tabSidebar];
    
    // ScrollView for tabs (max 6 tabs visible)
    CGFloat maxVisibleTabs = 6;
    CGFloat tabButtonHeight = 43;
    CGFloat tabSpacing = 3.6;
    CGFloat maxScrollHeight = maxVisibleTabs * (tabButtonHeight + tabSpacing) + 16; // 16 for padding
    
    self.tabScrollView = [[UIScrollView alloc] initWithFrame:CGRectMake(0, 0, sidebarWidth, self.tabSidebar.frame.size.height)];
    self.tabScrollView.showsVerticalScrollIndicator = YES; // Hiện scroll indicator bên phải
    self.tabScrollView.indicatorStyle = UIScrollViewIndicatorStyleWhite;
    self.tabScrollView.delegate = self;
    [self.tabSidebar addSubview:self.tabScrollView];
    
    // Container view for tabs
    self.tabContainerView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, sidebarWidth, 0)];
    [self.tabScrollView addSubview:self.tabContainerView];
    
    // Tùy chỉnh scroll indicator để bo góc và màu theo menu
    [self customizeScrollIndicator];
    
    // Vertical separator line
    UIView *separator = [[UIView alloc] initWithFrame:CGRectMake(sidebarWidth - 1, 0, 1, self.tabSidebar.frame.size.height)];
    separator.backgroundColor = [UIColor colorWithWhite:1.0 alpha:0.1];
    [self.tabSidebar addSubview:separator];
    
    // Initialize tab buttons array (tabs will be added via addTab: method)
    self.tabButtons = [NSMutableArray array];
    
    // Header - Top (left aligned relative to whole menu, không cộng sidebar)
    // Tính width khả dụng cho title (chừa chỗ cho các nút bên phải)
    CGFloat titleMaxWidth = self.frame.size.width - 120 - 15 - 15; // 15 padding trái + 15 padding phải
    // Căn chỉnh title và subtitle trong header (headerHeight = 50)
    CGFloat titleY = 10; // Top padding
    self.titleLabel = [[UILabel alloc] initWithFrame:CGRectMake(15, titleY, titleMaxWidth, 20)];
    self.titleLabel.text = @"BOORIRPG PREMIUM";
    self.titleLabel.textColor = [UIColor whiteColor];
    self.titleLabel.font = [UIFont systemFontOfSize:15 weight:UIFontWeightBold];
    self.titleLabel.textAlignment = NSTextAlignmentLeft;
    [self addSubview:self.titleLabel];
    
    // Social Buttons in Header (Top Right) - Centered vertically
    CGFloat buttonSize = 24;
    CGFloat headerCenterY = headerHeight / 2;
    CGFloat buttonY = headerCenterY - buttonSize / 2;
    
    self.telegramButton = [UIButton buttonWithType:UIButtonTypeCustom];
    self.telegramButton.frame = CGRectMake(self.frame.size.width - 110, buttonY, buttonSize, buttonSize);
    self.telegramButton.backgroundColor = [UIColor colorWithWhite:1.0 alpha:0.1];
    self.telegramButton.layer.cornerRadius = self.layer.cornerRadius * 0.3; // Scale with menu corner radius
    self.telegramButton.tag = 5001;
    [self.telegramButton setTitle:@"✈" forState:UIControlStateNormal];
    
    self.discordButton = [UIButton buttonWithType:UIButtonTypeCustom];
    self.discordButton.frame = CGRectMake(self.frame.size.width - 77, buttonY, buttonSize, buttonSize);
    self.discordButton.backgroundColor = [UIColor colorWithWhite:1.0 alpha:0.1];
    self.discordButton.layer.cornerRadius = self.layer.cornerRadius * 0.3;
    self.discordButton.tag = 5002;
    [self.discordButton setTitle:@"◈" forState:UIControlStateNormal];
    
    if ([[[UIDevice currentDevice] systemVersion] floatValue] >= 13.0) {
        [self.telegramButton setImage:[UIImage systemImageNamed:@"paperplane.fill"] forState:UIControlStateNormal];
        self.telegramButton.tintColor = [UIColor whiteColor];
        self.telegramButton.imageEdgeInsets = UIEdgeInsetsMake(5, 5, 5, 5);
        [self.telegramButton setTitle:@"" forState:UIControlStateNormal];
        
        [self.discordButton setImage:[UIImage systemImageNamed:@"bubble.left.and.bubble.right.fill"] forState:UIControlStateNormal];
        self.discordButton.tintColor = [UIColor whiteColor];
        self.discordButton.imageEdgeInsets = UIEdgeInsetsMake(5, 5, 5, 5);
        [self.discordButton setTitle:@"" forState:UIControlStateNormal];
    }
    
    [self.telegramButton addTarget:self action:@selector(socialTapped:) forControlEvents:UIControlEventTouchUpInside];
    [self.discordButton addTarget:self action:@selector(socialTapped:) forControlEvents:UIControlEventTouchUpInside];
    [self addSubview:self.telegramButton];
    [self addSubview:self.discordButton];

    self.closeButton = [UIButton buttonWithType:UIButtonTypeCustom];
    self.closeButton.frame = CGRectMake(self.frame.size.width - 40, buttonY, buttonSize, buttonSize);
    [self.closeButton setTitle:@"⤬" forState:UIControlStateNormal];
    [self.closeButton setTitleColor:[UIColor colorWithRed:1.0 green:0.23 blue:0.19 alpha:1.0] forState:UIControlStateNormal];
    self.closeButton.backgroundColor = [UIColor colorWithRed:1.0 green:0.23 blue:0.19 alpha:0.1];
    self.closeButton.layer.cornerRadius = self.layer.cornerRadius * 0.3;
    self.closeButton.titleLabel.font = [UIFont systemFontOfSize:18];
    [self.closeButton addTarget:self action:@selector(closeButtonTapped:) forControlEvents:UIControlEventTouchUpInside];
    [self addSubview:self.closeButton];
    
    // Subtitle ngay dưới title với khoảng cách đều
    CGFloat subtitleY = titleY + 20 + 2; // titleY + title height + spacing
    self.subtitleLabel = [[UILabel alloc] initWithFrame:CGRectMake(15, subtitleY, titleMaxWidth, 12)];
    self.subtitleLabel.text = @"ADVANCED SURVIVAL V2.5";
    self.subtitleLabel.textColor = [UIColor colorWithWhite:0.7 alpha:1.0];
    self.subtitleLabel.font = [UIFont systemFontOfSize:9 weight:UIFontWeightMedium];
    self.subtitleLabel.textAlignment = NSTextAlignmentLeft;
    [self addSubview:self.subtitleLabel];
    
    // Content ScrollView - Right side
    self.scrollView = [[UIScrollView alloc] initWithFrame:CGRectMake(contentStartX, headerHeight, contentWidth, self.frame.size.height - headerHeight - 30)];
    self.scrollView.showsVerticalScrollIndicator = YES;
    self.scrollView.indicatorStyle = UIScrollViewIndicatorStyleWhite;
    [self addSubview:self.scrollView];
    
    self.contentView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, contentWidth, 0)];
    [self.scrollView addSubview:self.contentView];
    
    // Footer Credit - Right side
    UILabel *footer = [[UILabel alloc] initWithFrame:CGRectMake(contentStartX, self.frame.size.height - 30, contentWidth, 25)];
    footer.textAlignment = NSTextAlignmentCenter;
    footer.textColor = [UIColor colorWithWhite:1.0 alpha:0.3];
    footer.font = [UIFont systemFontOfSize:10 weight:UIFontWeightBold];
    footer.text = @"CREDIT: X2NIOSVN";
    [self addSubview:footer];

    self.switches = [NSMutableDictionary dictionary];
    self.sliders = [NSMutableDictionary dictionary];
    self.sliderLabels = [NSMutableDictionary dictionary];
    self.buttons = [NSMutableDictionary dictionary];
    self.textFields = [NSMutableDictionary dictionary];
    
    // Default values
    self.canMove = NO;
}

- (void)socialTapped:(UIButton *)sender {
    NSString *urlStr = (sender.tag == 5001) ? self.telegramURL : self.discordURL;
    if (urlStr) {
        NSURL *url = [NSURL URLWithString:urlStr];
        if ([[UIApplication sharedApplication] canOpenURL:url]) {
            [[UIApplication sharedApplication] openURL:url options:@{} completionHandler:nil];
        }
    }
}

- (void)addThemeSlider:(NSString *)title property:(NSString *)prop max:(CGFloat)max min:(CGFloat)min value:(CGFloat)value handler:(void (^)(CGFloat value))handler {
    [self addSlider:title max:max min:min value:value handler:handler];
    UISlider *sl = self.sliders[title];
    objc_setAssociatedObject(sl, "themeProp", prop, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}

- (void)setFooterText:(NSString *)text {
    for (UIView *v in self.subviews) {
        if ([v isKindOfClass:[UILabel class]] && v.frame.origin.y > self.frame.size.height - 40) {
            ((UILabel *)v).text = text;
        }
    }
}

- (void)setMenuAccentColor:(UIColor *)color {
    self.accentColor = color;
    [self updateTheme];
    
    // Cập nhật màu của tất cả symbolLabel (◈) trong section titles
    for (UIView *container in self.contentView.subviews) {
        if (container.tag >= 1000) {
            UILabel *symbolLabel = [container viewWithTag:9998];
            if (symbolLabel) {
                symbolLabel.textColor = self.accentColor;
            }
            // Cập nhật màu của thanh gạch dọc trong switch
            UIView *verticalBar = [container viewWithTag:9997];
            if (verticalBar) {
                verticalBar.backgroundColor = self.accentColor;
            }
        }
    }
    
    // Cập nhật màu tab active và indicator ngay lập tức
    if (self.selectedTabIndex < self.tabButtons.count) {
        UIButton *activeTab = self.tabButtons[self.selectedTabIndex];
        activeTab.backgroundColor = [self.accentColor colorWithAlphaComponent:0.2];
        
        UIView *indicator = [activeTab viewWithTag:9999];
        if (indicator) {
            indicator.backgroundColor = self.accentColor;
        }
    }
}

- (void)setMenuGlassEffect:(BOOL)enabled {
    self.blurEffectView.hidden = !enabled;
}

- (void)setMenuCornerRadius:(CGFloat)radius {
    self.layer.cornerRadius = radius;
    
    // Update header buttons corner radius proportionally
    CGFloat buttonCornerRadius = radius * 0.3;
    if (self.telegramButton) {
        self.telegramButton.layer.cornerRadius = buttonCornerRadius;
    }
    if (self.discordButton) {
        self.discordButton.layer.cornerRadius = buttonCornerRadius;
    }
    if (self.closeButton) {
        self.closeButton.layer.cornerRadius = buttonCornerRadius;
    }
    
    // Update vertical bars in switches corner radius proportionally (bo góc mềm hơn)
    CGFloat barCornerRadius = MAX(radius * 0.5, 1.5); // Tỷ lệ lớn hơn và có giá trị tối thiểu
    for (UIView *container in self.contentView.subviews) {
        if (container.tag >= 1000) {
            UIView *verticalBar = [container viewWithTag:9997];
            if (verticalBar) {
                verticalBar.layer.cornerRadius = barCornerRadius;
            }
        }
    }
    
    // Update tab buttons corner radius proportionally
    CGFloat tabButtonCornerRadius = radius * 0.3;
    for (UIButton *tabBtn in self.tabButtons) {
        tabBtn.layer.cornerRadius = tabButtonCornerRadius;
        
        // Update indicator corner radius (proportional to its height)
        UIView *indicator = [tabBtn viewWithTag:9999];
        if (indicator) {
            // Indicator height is 2px, so use a small proportional radius
            indicator.layer.cornerRadius = radius * 0.1;
        }
    }
    
    // Update all buttons corner radius (combo, regular buttons)
    for (UIButton *btn in self.buttons.allValues) {
        btn.layer.cornerRadius = buttonCornerRadius;
    }
    
    // Update all text fields corner radius
    for (UITextField *field in self.textFields.allValues) {
        field.layer.cornerRadius = buttonCornerRadius;
    }
    
    // Update combo selectors corner radius (they're in contentView)
    for (UIView *container in self.contentView.subviews) {
        if (container.tag >= 1000) {
            for (UIView *subview in container.subviews) {
                if ([subview isKindOfClass:[UIButton class]]) {
                    UIButton *comboBtn = (UIButton *)subview;
                    if ([comboBtn.subviews count] > 0) {
                        // Check if it has arrow label (combo selector)
                        BOOL isCombo = NO;
                        for (UIView *sv in comboBtn.subviews) {
                            if ([sv isKindOfClass:[UILabel class]] && [((UILabel *)sv).text isEqualToString:@"▼"]) {
                                isCombo = YES;
                                break;
                            }
                        }
                        if (isCombo) {
                            comboBtn.layer.cornerRadius = buttonCornerRadius;
                        }
                    }
                } else if ([subview isKindOfClass:[UITextField class]]) {
                    subview.layer.cornerRadius = buttonCornerRadius;
                }
            }
        }
    }
}

- (void)setMenuBorderWidth:(CGFloat)width {
    self.layer.borderWidth = width;
}

- (void)setMenuTitle:(NSString *)title {
    self.titleLabel.text = title;
    self.titleLabel.textAlignment = NSTextAlignmentLeft;
}

- (void)setMenuSubtitle:(NSString *)subtitle {
    self.subtitleLabel.text = subtitle;
    self.subtitleLabel.textAlignment = NSTextAlignmentLeft;
}

- (void)updateTheme {
    for (UISwitch *s in self.switches.allValues) {
        s.onTintColor = self.accentColor;
    }
    for (UISlider *sl in self.sliders.allValues) {
        sl.minimumTrackTintColor = self.accentColor;
    }
    for (UILabel *lbl in self.sliderLabels.allValues) {
        lbl.textColor = self.accentColor;
    }
    for (UIButton *btn in self.buttons.allValues) {
        if (![btn.currentTitle isEqualToString:@"⤬"]) {
            btn.backgroundColor = self.accentColor;
        }
    }
}

- (void)tabButtonTapped:(UIButton *)sender {
    NSInteger newIndex = sender.tag;
    if (newIndex == self.selectedTabIndex) return;
    
    // Get text size for indicator width (centered)
    CGSize textSize = [sender.titleLabel.text sizeWithAttributes:@{NSFontAttributeName: sender.titleLabel.font}];
    CGFloat indicatorWidth = textSize.width;
    CGFloat tabButtonHeight = sender.frame.size.height;
    CGFloat buttonWidth = sender.frame.size.width;
    CGFloat textX = (buttonWidth - indicatorWidth) / 2; // Center the indicator
    
    // Animate tab change
    UIButton *prevBtn = self.tabButtons[self.selectedTabIndex];
    UIView *prevIndicator = [prevBtn viewWithTag:9999];
    
    // Animate previous indicator shrinking from center
    [UIView animateWithDuration:0.2 animations:^{
        prevBtn.selected = NO;
        // Tab unactive không có nền
        prevBtn.backgroundColor = [UIColor clearColor];
        [prevBtn setTitleColor:[UIColor colorWithWhite:1.0 alpha:0.5] forState:UIControlStateNormal];
        prevBtn.transform = CGAffineTransformIdentity;
        
        // Shrink previous indicator from center
        CGRect prevFrame = prevIndicator.frame;
        CGFloat prevCenterX = prevFrame.origin.x + prevFrame.size.width / 2;
        prevIndicator.frame = CGRectMake(prevCenterX, prevFrame.origin.y, 0, prevFrame.size.height);
        prevIndicator.alpha = 0;
    } completion:^(BOOL finished) {
        prevIndicator.hidden = YES;
    }];
    
    // Select new tab with animation
    sender.selected = YES;
    // Nền mờ theo màu accent của menu
    sender.backgroundColor = [self.accentColor colorWithAlphaComponent:0.2];
    [sender setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
    
    UIView *indicator = [sender viewWithTag:9999];
    indicator.hidden = NO;
    indicator.backgroundColor = self.accentColor;
    
    // Start indicator from center and expand outward
    CGFloat centerX = buttonWidth / 2;
    indicator.frame = CGRectMake(centerX, tabButtonHeight - 2, 0, 2);
    indicator.alpha = 1.0;
    
    [UIView animateWithDuration:0.3 delay:0 usingSpringWithDamping:0.7 initialSpringVelocity:0.5 options:UIViewAnimationOptionCurveEaseOut animations:^{
        sender.transform = CGAffineTransformMakeScale(1.02, 1.02);
        // Expand indicator from center to full width
        indicator.frame = CGRectMake(textX, tabButtonHeight - 2, indicatorWidth, 2);
    } completion:^(BOOL finished) {
        [UIView animateWithDuration:0.15 animations:^{
            sender.transform = CGAffineTransformIdentity;
        }];
    }];
    
    self.selectedTabIndex = newIndex;
    [self endEditing:YES];
    
    // Animate content transition
    self.contentView.alpha = 0.3;
    [UIView animateWithDuration:0.25 animations:^{
        self.contentView.alpha = 1.0;
    }];
    
    [self updateLayout];
}

- (void)tabChanged:(UISegmentedControl *)sender {
    [self endEditing:YES];
    self.selectedTabIndex = sender.selectedSegmentIndex;
    [self updateLayout];
}

- (void)addSectionTitle:(NSString *)title {
    CGFloat contentWidth = self.scrollView.frame.size.width;
    UIView *container = [[UIView alloc] initWithFrame:CGRectMake(0, 0, contentWidth, 20)];
    container.tag = self.currentCategoryCounter + 1000;
    
    // Ký tự ◈ màu accent ở đầu
    UILabel *symbolLabel = [[UILabel alloc] initWithFrame:CGRectMake(16, 3, 12, 16)];
    symbolLabel.text = @"◈";
    symbolLabel.textColor = self.accentColor;
    symbolLabel.font = [UIFont systemFontOfSize:10 weight:UIFontWeightBold];
    symbolLabel.tag = 9998; // Tag để tìm và cập nhật màu sau này
    [container addSubview:symbolLabel];
    
    UILabel *label = [[UILabel alloc] initWithFrame:CGRectMake(16 + 12 + 4, 3, 0, 16)];
    label.text = [title uppercaseString];
    label.textColor = [UIColor colorWithWhite:0.6 alpha:1.0];
    label.font = [UIFont systemFontOfSize:8 weight:UIFontWeightBold];
    CGSize textSize = [label sizeThatFits:CGSizeMake(contentWidth - 32 - 16, 16)];
    label.frame = CGRectMake(16 + 12 + 4, 3, textSize.width, 16);
    [container addSubview:label];
    
    // Tính lại vị trí line sau khi thêm ký tự ◈
    CGFloat textStartX = 16 + 12 + 4;
    UIView *line = [[UIView alloc] initWithFrame:CGRectMake(textStartX + textSize.width + 8, 11, contentWidth - (textStartX + textSize.width + 8) - 16, 1)];
    line.backgroundColor = [UIColor colorWithWhite:1.0 alpha:0.05];
    [container addSubview:line];
    
    [self.contentView addSubview:container];
    [self updateLayout];
}

- (void)setTabIndex:(NSInteger)index {
    self.currentCategoryCounter = index;
}

- (void)addFeatureSwitch:(NSString *)title description:(NSString *)desc handler:(void (^)(BOOL isOn))handler {
    CGFloat contentWidth = self.scrollView.frame.size.width;
    UIView *container = [[UIView alloc] initWithFrame:CGRectMake(16, 0, contentWidth - 32, 40)];
    container.tag = self.currentCategoryCounter + 1000;
    
    // Thanh gạch dọc trước tên và mô tả
    CGFloat barWidth = 3.0;
    CGFloat containerHeight = 40.0;
    CGFloat barHeight = 28.0; // Chiều cao đủ để bao phủ cả label và descLabel
    CGFloat barX = 0;
    CGFloat barY = (containerHeight - barHeight) / 2.0; // Căn giữa theo chiều dọc
    UIView *verticalBar = [[UIView alloc] initWithFrame:CGRectMake(barX, barY, barWidth, barHeight)];
    verticalBar.backgroundColor = self.accentColor;
    // Bo góc mềm hơn - dùng tỷ lệ lớn hơn hoặc giá trị tối thiểu
    CGFloat barCornerRadius = MAX(self.layer.cornerRadius * 0.5, 1.5);
    verticalBar.layer.cornerRadius = barCornerRadius;
    verticalBar.tag = 9997; // Tag để cập nhật màu và corner radius sau này
    [container addSubview:verticalBar];
    
    // Điều chỉnh vị trí label và descLabel để có khoảng cách với thanh gạch và căn giữa với thanh dọc
    CGFloat textOffset = barWidth + 8; // Khoảng cách từ thanh gạch đến text
    CGFloat labelY = barY + 2; // Căn với thanh dọc (barY + một chút padding)
    CGFloat descY = barY + barHeight - 12 - 2; // Căn với phần dưới của thanh dọc
    
    UILabel *label = [[UILabel alloc] initWithFrame:CGRectMake(textOffset, labelY, container.frame.size.width - 48 - textOffset, 16)];
    label.text = title;
    label.textColor = [UIColor whiteColor];
    label.font = [UIFont systemFontOfSize:12 weight:UIFontWeightSemibold];
    [container addSubview:label];
    
    UILabel *descLabel = [[UILabel alloc] initWithFrame:CGRectMake(textOffset, descY, container.frame.size.width - 48 - textOffset, 12)];
    descLabel.text = desc;
    descLabel.textColor = [UIColor colorWithWhite:0.6 alpha:1.0];
    descLabel.font = [UIFont systemFontOfSize:9];
    [container addSubview:descLabel];
    
    // Switch căn giữa với thanh dọc
    CGFloat switchY = (containerHeight - 31) / 2.0; // Căn giữa switch với container
    UISwitch *toggle = [[UISwitch alloc] initWithFrame:CGRectMake(container.frame.size.width - 51 + 2, switchY, 51, 31)];
    toggle.transform = CGAffineTransformMakeScale(0.8, 0.8);
    toggle.onTintColor = self.accentColor;
    [toggle addTarget:self action:@selector(switchChanged:) forControlEvents:UIControlEventValueChanged];
    objc_setAssociatedObject(toggle, "switchHandler", handler, OBJC_ASSOCIATION_COPY_NONATOMIC);
    [container addSubview:toggle];
    
    [self.contentView addSubview:container];
    self.switches[title] = toggle;
    [self updateLayout];
}

- (void)addSlider:(NSString *)title max:(CGFloat)max min:(CGFloat)min value:(CGFloat)value handler:(void (^)(CGFloat value))handler {
    CGFloat contentWidth = self.scrollView.frame.size.width;
    UIView *container = [[UIView alloc] initWithFrame:CGRectMake(16, 0, contentWidth - 32, 48)];
    container.tag = self.currentCategoryCounter + 1000;
    
    UILabel *titleLabel = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, container.frame.size.width - 80, 16)];
    titleLabel.text = title;
    titleLabel.textColor = [UIColor whiteColor];
    titleLabel.font = [UIFont systemFontOfSize:11 weight:UIFontWeightMedium];
    [container addSubview:titleLabel];
    
    UILabel *valueLabel = [[UILabel alloc] initWithFrame:CGRectMake(container.frame.size.width - 50, 0, 50, 16)];
    valueLabel.textColor = self.accentColor;
    valueLabel.font = [UIFont systemFontOfSize:10 weight:UIFontWeightBold];
    valueLabel.textAlignment = NSTextAlignmentRight;
    valueLabel.text = [NSString stringWithFormat:@"%.1f", value];
    [container addSubview:valueLabel];
    
    UISlider *slider = [[UISlider alloc] initWithFrame:CGRectMake(0, 20, container.frame.size.width, 24)];
    slider.minimumValue = min;
    slider.maximumValue = max;
    slider.value = value;
    slider.minimumTrackTintColor = self.accentColor;
    
    // Thu nhỏ thumb (nút tròn) 50% bằng cách tạo custom thumb image
    UIImage *thumbImage = [self createSliderThumbImage];
    [slider setThumbImage:thumbImage forState:UIControlStateNormal];
    [slider setThumbImage:thumbImage forState:UIControlStateHighlighted];
    
    [slider addTarget:self action:@selector(sliderValueChanged:) forControlEvents:UIControlEventValueChanged];
    objc_setAssociatedObject(slider, "sliderHandler", handler, OBJC_ASSOCIATION_COPY_NONATOMIC);
    [container addSubview:slider];
    
    [self.contentView addSubview:container];
    self.sliders[title] = slider;
    self.sliderLabels[title] = valueLabel;
    [self updateLayout];
}

- (void)addComboSelector:(NSString *)title options:(NSArray *)options selectedIndex:(NSInteger)index handler:(void (^)(NSInteger selectedIndex))handler {
    CGFloat contentWidth = self.scrollView.frame.size.width;
    UIView *container = [[UIView alloc] initWithFrame:CGRectMake(16, 0, contentWidth - 32, 56)];
    container.tag = self.currentCategoryCounter + 1000;
    
    UILabel *titleLabel = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, container.frame.size.width, 16)];
    titleLabel.text = title;
    titleLabel.textColor = [UIColor whiteColor];
    titleLabel.font = [UIFont systemFontOfSize:11 weight:UIFontWeightMedium];
    [container addSubview:titleLabel];
    
    UIButton *comboBtn = [UIButton buttonWithType:UIButtonTypeCustom];
    comboBtn.frame = CGRectMake(0, 20, container.frame.size.width, 32);
    comboBtn.backgroundColor = [UIColor colorWithWhite:1.0 alpha:0.05];
    comboBtn.layer.cornerRadius = self.layer.cornerRadius * 0.3;
    comboBtn.layer.borderWidth = 1.0;
    comboBtn.layer.borderColor = [UIColor colorWithWhite:1.0 alpha:0.1].CGColor;
    [comboBtn setTitle:options[index] forState:UIControlStateNormal];
    [comboBtn setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
    comboBtn.titleLabel.font = [UIFont systemFontOfSize:10];
    comboBtn.contentHorizontalAlignment = UIControlContentHorizontalAlignmentLeft;
    comboBtn.titleEdgeInsets = UIEdgeInsetsMake(0, 12, 0, 0);
    
    // Arrow icon (mặc định quay ngang ▶) - chỉ arrow mới có thể mở dropdown
    UIButton *arrowBtn = [UIButton buttonWithType:UIButtonTypeCustom];
    arrowBtn.frame = CGRectMake(comboBtn.frame.size.width - 24, 0, 20, 32);
    [arrowBtn setTitle:@"▶" forState:UIControlStateNormal];
    [arrowBtn setTitleColor:[UIColor colorWithWhite:0.5 alpha:1.0] forState:UIControlStateNormal];
    arrowBtn.titleLabel.font = [UIFont systemFontOfSize:11];
    arrowBtn.tag = 8888; // Tag để tìm arrow
    [arrowBtn addTarget:self action:@selector(comboTapped:) forControlEvents:UIControlEventTouchUpInside];
    [comboBtn addSubview:arrowBtn];
    
    // Close button với dấu ⤬ màu đỏ (ẩn mặc định)
    UIButton *closeBtn = [UIButton buttonWithType:UIButtonTypeCustom];
    closeBtn.frame = CGRectMake(comboBtn.frame.size.width - 24, 0, 20, 32);
    [closeBtn setTitle:@"⤬" forState:UIControlStateNormal];
    [closeBtn setTitleColor:[UIColor colorWithRed:1.0 green:0.23 blue:0.19 alpha:1.0] forState:UIControlStateNormal];
    closeBtn.titleLabel.font = [UIFont systemFontOfSize:15];
    closeBtn.hidden = YES;
    closeBtn.tag = 8889; // Tag để tìm close button
    [closeBtn addTarget:self action:@selector(comboCloseTapped:) forControlEvents:UIControlEventTouchUpInside];
    [comboBtn addSubview:closeBtn];
    objc_setAssociatedObject(comboBtn, "comboOptions", options, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    objc_setAssociatedObject(comboBtn, "comboHandler", handler, OBJC_ASSOCIATION_COPY_NONATOMIC);
    objc_setAssociatedObject(comboBtn, "comboSelectedIndex", @(index), OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    objc_setAssociatedObject(comboBtn, "comboArrow", arrowBtn, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    objc_setAssociatedObject(comboBtn, "comboCloseBtn", closeBtn, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    
    [container addSubview:comboBtn];
    [self.contentView addSubview:container];
    [self updateLayout];
}

- (void)comboTapped:(UIButton *)sender {
    // sender là arrowBtn, cần tìm comboBtn cha
    UIButton *comboBtn = nil;
    if (sender.tag == 8888) {
        // sender là arrow button, tìm comboBtn cha
        for (UIView *container in self.contentView.subviews) {
            if (container.tag >= 1000) {
                for (UIView *subview in container.subviews) {
                    if ([subview isKindOfClass:[UIButton class]]) {
                        UIButton *foundArrow = objc_getAssociatedObject(subview, "comboArrow");
                        if (foundArrow == sender) {
                            comboBtn = (UIButton *)subview;
                            break;
                        }
                    }
                }
                if (comboBtn) break;
            }
        }
    } else {
        comboBtn = sender;
    }
    
    if (!comboBtn) return;
    
    NSArray *options = objc_getAssociatedObject(comboBtn, "comboOptions");
    void (^handler)(NSInteger) = objc_getAssociatedObject(comboBtn, "comboHandler");
    UIButton *arrow = objc_getAssociatedObject(comboBtn, "comboArrow");
    
    // Kiểm tra xem dropdown đã mở chưa
    UIView *existingDropdown = objc_getAssociatedObject(comboBtn, "comboDropdown");
    if (existingDropdown && existingDropdown.superview) {
        // Đóng dropdown nếu đã mở
        [self closeComboDropdown:comboBtn];
        return;
    }
    
    // Đóng tất cả dropdown khác
    [self closeAllComboDropdowns];
    
    // Ẩn arrow và hiện close button (dấu ⤬ màu đỏ)
    UIButton *closeBtn = objc_getAssociatedObject(comboBtn, "comboCloseBtn");
    [UIView animateWithDuration:0.2 animations:^{
        arrow.alpha = 0;
        closeBtn.alpha = 0;
    } completion:^(BOOL finished) {
        arrow.hidden = YES;
        closeBtn.hidden = NO;
        closeBtn.alpha = 1.0;
    }];
    
    // Tạo dropdown menu
    CGFloat dropdownWidth = comboBtn.frame.size.width;
    CGFloat itemHeight = 32;
    CGFloat maxHeight = 160; // Max 5 items visible
    CGFloat dropdownHeight = MIN(options.count * itemHeight, maxHeight);
    
    // Tìm container để tính toán vị trí dropdown
    UIView *container = comboBtn.superview;
    CGPoint containerPoint = [container convertPoint:CGPointMake(comboBtn.frame.origin.x, comboBtn.frame.origin.y + comboBtn.frame.size.height) toView:self.contentView];
    UIView *dropdown = [[UIView alloc] initWithFrame:CGRectMake(containerPoint.x, containerPoint.y, dropdownWidth, dropdownHeight)];
    dropdown.backgroundColor = [UIColor colorWithWhite:0.1 alpha:0.95];
    dropdown.layer.cornerRadius = self.layer.cornerRadius * 0.3;
    dropdown.layer.borderWidth = 1.0;
    dropdown.layer.borderColor = [UIColor colorWithWhite:1.0 alpha:0.2].CGColor;
    dropdown.layer.shadowColor = [UIColor blackColor].CGColor;
    dropdown.layer.shadowOffset = CGSizeMake(0, 4);
    dropdown.layer.shadowOpacity = 0.3;
    dropdown.layer.shadowRadius = 8;
    dropdown.tag = 7777; // Tag để identify dropdown
    
    // Scroll view nếu có nhiều options
    UIScrollView *scrollView = nil;
    if (options.count > 5) {
        scrollView = [[UIScrollView alloc] initWithFrame:dropdown.bounds];
        scrollView.contentSize = CGSizeMake(dropdownWidth, options.count * itemHeight);
        scrollView.showsVerticalScrollIndicator = YES;
        scrollView.indicatorStyle = UIScrollViewIndicatorStyleWhite;
        [dropdown addSubview:scrollView];
    }
    
    UIView *itemsContainer = scrollView ? scrollView : dropdown;
    CGFloat containerY = scrollView ? 0 : 0;
    
    for (NSInteger i = 0; i < options.count; i++) {
        UIButton *optionBtn = [UIButton buttonWithType:UIButtonTypeCustom];
        optionBtn.frame = CGRectMake(0, containerY + (i * itemHeight), dropdownWidth, itemHeight - 1);
        [optionBtn setTitle:options[i] forState:UIControlStateNormal];
        [optionBtn setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
        optionBtn.titleLabel.font = [UIFont systemFontOfSize:10];
        optionBtn.contentHorizontalAlignment = UIControlContentHorizontalAlignmentLeft;
        optionBtn.titleEdgeInsets = UIEdgeInsetsMake(0, 12, 0, 0);
        optionBtn.tag = i;
        
        // Highlight selected option
        NSNumber *selectedIndex = objc_getAssociatedObject(comboBtn, "comboSelectedIndex");
        if (selectedIndex && selectedIndex.integerValue == i) {
            optionBtn.backgroundColor = [self.accentColor colorWithAlphaComponent:0.2];
        } else {
            optionBtn.backgroundColor = [UIColor clearColor];
        }
        
        [optionBtn addTarget:self action:@selector(comboOptionSelected:) forControlEvents:UIControlEventTouchUpInside];
        [itemsContainer addSubview:optionBtn];
        
        // Separator line (trừ item cuối)
        if (i < options.count - 1) {
            UIView *separator = [[UIView alloc] initWithFrame:CGRectMake(12, itemHeight - 1, dropdownWidth - 24, 1)];
            separator.backgroundColor = [UIColor colorWithWhite:1.0 alpha:0.1];
            [optionBtn addSubview:separator];
        }
    }
    
    // Đảm bảo dropdown không vượt quá contentView
    CGFloat maxY = self.contentView.frame.size.height;
    if (dropdown.frame.origin.y + dropdownHeight > maxY) {
        // Hiển thị phía trên button
        dropdown.frame = CGRectMake(containerPoint.x, containerPoint.y - dropdownHeight - comboBtn.frame.size.height, dropdownWidth, dropdownHeight);
    }
    
    [self.contentView addSubview:dropdown];
    objc_setAssociatedObject(comboBtn, "comboDropdown", dropdown, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    
    // Animation fade in
    dropdown.alpha = 0;
    dropdown.transform = CGAffineTransformMakeScale(0.95, 0.95);
    [UIView animateWithDuration:0.2 animations:^{
        dropdown.alpha = 1.0;
        dropdown.transform = CGAffineTransformIdentity;
    }];
    
    // Tap outside để đóng
    UITapGestureRecognizer *tapOutside = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(closeComboDropdownsOnTap:)];
    tapOutside.cancelsTouchesInView = NO;
    [self.scrollView addGestureRecognizer:tapOutside];
    objc_setAssociatedObject(dropdown, "tapOutsideGesture", tapOutside, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}

- (void)comboOptionSelected:(UIButton *)optionBtn {
    // Tìm combo button cha
    UIView *dropdown = optionBtn.superview;
    while (dropdown && dropdown.tag != 7777) {
        dropdown = dropdown.superview;
    }
    if (!dropdown) return;
    
    // Tìm combo button
    UIButton *comboBtn = nil;
    for (UIView *container in self.contentView.subviews) {
        if (container.tag >= 1000) {
            for (UIView *subview in container.subviews) {
                if ([subview isKindOfClass:[UIButton class]]) {
                    UIView *foundDropdown = objc_getAssociatedObject(subview, "comboDropdown");
                    if (foundDropdown == dropdown) {
                        comboBtn = (UIButton *)subview;
                        break;
                    }
                }
            }
            if (comboBtn) break;
        }
    }
    
    if (comboBtn) {
        NSArray *options = objc_getAssociatedObject(comboBtn, "comboOptions");
        void (^handler)(NSInteger) = objc_getAssociatedObject(comboBtn, "comboHandler");
        NSInteger selectedIndex = optionBtn.tag;
        
        // Update button title
        [comboBtn setTitle:options[selectedIndex] forState:UIControlStateNormal];
        objc_setAssociatedObject(comboBtn, "comboSelectedIndex", @(selectedIndex), OBJC_ASSOCIATION_RETAIN_NONATOMIC);
        
        // Call handler
        if (handler) handler(selectedIndex);
        
        // Đóng dropdown
        [self closeComboDropdown:comboBtn];
    }
}

- (void)closeComboDropdown:(UIButton *)comboBtn {
    UIView *dropdown = objc_getAssociatedObject(comboBtn, "comboDropdown");
    UILabel *arrow = objc_getAssociatedObject(comboBtn, "comboArrow");
    UIButton *closeBtn = objc_getAssociatedObject(comboBtn, "comboCloseBtn");
    
    if (dropdown) {
        // Remove tap gesture
        UITapGestureRecognizer *tapGesture = objc_getAssociatedObject(dropdown, "tapOutsideGesture");
        if (tapGesture) {
            [self.scrollView removeGestureRecognizer:tapGesture];
        }
        
        // Animation fade out
        [UIView animateWithDuration:0.2 animations:^{
            dropdown.alpha = 0;
            dropdown.transform = CGAffineTransformMakeScale(0.95, 0.95);
        } completion:^(BOOL finished) {
            [dropdown removeFromSuperview];
        }];
        
        objc_setAssociatedObject(comboBtn, "comboDropdown", nil, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    }
    
    // Ẩn close button (dấu ⤬) và hiện lại arrow (▶)
    [UIView animateWithDuration:0.2 animations:^{
        closeBtn.alpha = 0;
        arrow.alpha = 0;
    } completion:^(BOOL finished) {
        closeBtn.hidden = YES;
        arrow.hidden = NO;
        arrow.alpha = 1.0;
    }];
}

- (void)comboCloseTapped:(UIButton *)sender {
    // Tìm combo button cha
    UIButton *comboBtn = nil;
    for (UIView *container in self.contentView.subviews) {
        if (container.tag >= 1000) {
            for (UIView *subview in container.subviews) {
                if ([subview isKindOfClass:[UIButton class]]) {
                    UIButton *foundCloseBtn = objc_getAssociatedObject(subview, "comboCloseBtn");
                    if (foundCloseBtn == sender) {
                        comboBtn = (UIButton *)subview;
                        break;
                    }
                }
            }
            if (comboBtn) break;
        }
    }
    
    if (comboBtn) {
        [self closeComboDropdown:comboBtn];
    }
}

- (void)closeAllComboDropdowns {
    for (UIView *container in self.contentView.subviews) {
        if (container.tag >= 1000) {
            for (UIView *subview in container.subviews) {
                if ([subview isKindOfClass:[UIButton class]]) {
                    UIView *dropdown = objc_getAssociatedObject(subview, "comboDropdown");
                    if (dropdown && dropdown.superview) {
                        [self closeComboDropdown:(UIButton *)subview];
                    }
                }
            }
        }
    }
}

- (void)closeComboDropdownsOnTap:(UITapGestureRecognizer *)gesture {
    CGPoint location = [gesture locationInView:self.scrollView];
    
    // Kiểm tra xem tap có phải trong dropdown không
    BOOL tappedInDropdown = NO;
    for (UIView *subview in self.contentView.subviews) {
        if (subview.tag == 7777 && CGRectContainsPoint([self.contentView convertRect:subview.frame toView:self.scrollView], location)) {
            tappedInDropdown = YES;
            break;
        }
    }
    
    if (!tappedInDropdown) {
        [self closeAllComboDropdowns];
    }
}

- (void)addTextField:(NSString *)title placeholder:(NSString *)placeholder handler:(void (^)(NSString *text))handler {
    CGFloat contentWidth = self.scrollView.frame.size.width;
    UIView *container = [[UIView alloc] initWithFrame:CGRectMake(16, 0, contentWidth - 32, 56)];
    container.tag = self.currentCategoryCounter + 1000;
    
    UILabel *titleLabel = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, container.frame.size.width, 16)];
    titleLabel.text = title;
    titleLabel.textColor = [UIColor whiteColor];
    titleLabel.font = [UIFont systemFontOfSize:11 weight:UIFontWeightMedium];
    [container addSubview:titleLabel];
    
    UITextField *field = [[UITextField alloc] initWithFrame:CGRectMake(0, 20, container.frame.size.width, 32)];
    field.backgroundColor = [UIColor colorWithWhite:1.0 alpha:0.05];
    field.layer.cornerRadius = self.layer.cornerRadius * 0.3;
    field.layer.borderWidth = 1.0;
    field.layer.borderColor = [UIColor colorWithWhite:1.0 alpha:0.1].CGColor;
    field.textColor = [UIColor whiteColor];
    field.font = [UIFont systemFontOfSize:10];
    field.attributedPlaceholder = [[NSAttributedString alloc] initWithString:placeholder attributes:@{NSForegroundColorAttributeName: [UIColor colorWithWhite:1.0 alpha:0.4]}];
    field.delegate = self;
    field.returnKeyType = UIReturnKeyDone;
    
    // Padding
    UIView *paddingView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, 12, 32)];
    field.leftView = paddingView;
    field.leftViewMode = UITextFieldViewModeAlways;
    
    objc_setAssociatedObject(field, "fieldHandler", handler, OBJC_ASSOCIATION_COPY_NONATOMIC);
    [container addSubview:field];
    [self.contentView addSubview:container];
    [self updateLayout];
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField {
    [textField resignFirstResponder];
    void (^handler)(NSString *) = objc_getAssociatedObject(textField, "fieldHandler");
    if (handler) handler(textField.text);
    return YES;
}

- (void)addButton:(NSString *)title withHandler:(void (^)(void))handler {
    CGFloat contentWidth = self.scrollView.frame.size.width;
    UIButton *button = [UIButton buttonWithType:UIButtonTypeCustom];
    button.frame = CGRectMake(16, 0, contentWidth - 32, 36);
    button.tag = self.currentCategoryCounter + 1000;
    [button setTitle:[title uppercaseString] forState:UIControlStateNormal];
    [button setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
    button.titleLabel.font = [UIFont systemFontOfSize:11 weight:UIFontWeightBold];
    button.backgroundColor = [UIColor colorWithRed:110.0/255.0 green:142.0/255.0 blue:251.0/255.0 alpha:1.0];
    button.layer.cornerRadius = self.layer.cornerRadius * 0.3;
    objc_setAssociatedObject(button, "buttonHandler", handler, OBJC_ASSOCIATION_COPY_NONATOMIC);
    [button addTarget:self action:@selector(buttonTapped:) forControlEvents:UIControlEventTouchUpInside];
    [self.contentView addSubview:button];
    self.buttons[title] = button;
    [self updateLayout];
}

- (void)updateLayout {
    NSInteger selectedTab = self.selectedTabIndex;
    CGFloat contentWidth = self.scrollView.frame.size.width;
    __block CGFloat yOffset = 8;
    [self.contentView.subviews enumerateObjectsUsingBlock:^(__kindof UIView * _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
        if (obj.tag >= 1000) {
            if (obj.tag - 1000 == selectedTab) {
                obj.hidden = NO;
                CGRect frame = obj.frame;
                frame.origin.y = yOffset;
                // Update width to match content area
                if (frame.size.width > contentWidth - 32) {
                    frame.size.width = contentWidth - 32;
                }
                obj.frame = frame;
                yOffset += frame.size.height + 6;
            } else { obj.hidden = YES; }
        }
    }];
    self.contentView.frame = CGRectMake(0, 0, contentWidth, yOffset);
    self.scrollView.contentSize = CGSizeMake(contentWidth, yOffset);
}

- (void)switchChanged:(UISwitch *)sender {
    void (^handler)(BOOL) = objc_getAssociatedObject(sender, "switchHandler");
    if (handler) handler(sender.isOn);
}

- (void)sliderValueChanged:(UISlider *)slider {
    for (NSString *key in self.sliders) {
        if (self.sliders[key] == slider) {
            UILabel *label = (UILabel *)self.sliderLabels[key];
            label.text = [NSString stringWithFormat:@"%.1f", slider.value];
            
            void (^handler)(CGFloat) = objc_getAssociatedObject(slider, "sliderHandler");
            if (handler) handler(slider.value);

            NSString *prop = objc_getAssociatedObject(slider, "themeProp");
            if ([prop isEqualToString:@"opacity"]) self.alpha = slider.value;
            if ([prop isEqualToString:@"corner"]) [self setMenuCornerRadius:slider.value];
            if ([prop isEqualToString:@"border"]) [self setMenuBorderWidth:slider.value];
            break;
        }
    }
}

- (void)buttonTapped:(UIButton *)sender {
    [UIView animateWithDuration:0.1 animations:^{ sender.transform = CGAffineTransformMakeScale(0.96, 0.96); } completion:^(BOOL finished) {
        [UIView animateWithDuration:0.1 animations:^{ sender.transform = CGAffineTransformIdentity; } completion:^(BOOL finished) {
            void (^handler)(void) = objc_getAssociatedObject(sender, "buttonHandler");
            if (handler) handler();
        }];
    }];
}

- (void)close {
    [UIView animateWithDuration:0.3 animations:^{ self.alpha = 0; } completion:^(BOOL finished) { [self removeFromSuperview]; }];
}

- (void)makeDraggable {
    if (self.panGesture) {
        [self removeGestureRecognizer:self.panGesture];
    }
    if (self.canMove) {
        self.panGesture = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(handlePan:)];
        self.panGesture.delegate = self;
        self.panGesture.cancelsTouchesInView = NO;
        [self addGestureRecognizer:self.panGesture];
    }
}

- (void)canMove:(BOOL)enabled {
    self.canMove = enabled;
    [self makeDraggable];
    
    // Nếu canMove = false, đặt menu ở giữa màn hình
    if (!enabled) {
        UIWindow *window = [UIApplication sharedApplication].keyWindow;
        if (window) {
            CGSize screenSize = window.bounds.size;
            CGRect menuFrame = self.frame;
            menuFrame.origin.x = (screenSize.width - menuFrame.size.width) / 2;
            menuFrame.origin.y = (screenSize.height - menuFrame.size.height) / 2;
            self.frame = menuFrame;
        }
    }
}

- (BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldReceiveTouch:(UITouch *)touch {
    // Don't allow pan gesture if touch is on interactive controls
    UIView *touchedView = touch.view;
    
    // Check if touch is on a slider
    if ([touchedView isKindOfClass:[UISlider class]]) {
        return NO;
    }
    
    // Check if touch is on a button (except header buttons which should allow dragging)
    if ([touchedView isKindOfClass:[UIButton class]]) {
        UIButton *btn = (UIButton *)touchedView;
        // Allow dragging if it's header buttons (telegram, discord, close)
        if (btn.tag == 5001 || btn.tag == 5002 || btn == self.closeButton) {
            return NO; // Don't drag when tapping header buttons
        }
        // For other buttons, don't allow dragging
        return NO;
    }
    
    // Check if touch is on a switch
    if ([touchedView isKindOfClass:[UISwitch class]]) {
        return NO;
    }
    
    // Check if touch is inside scroll view content area (allow scrolling)
    CGPoint touchPoint = [touch locationInView:self];
    if (CGRectContainsPoint(self.scrollView.frame, touchPoint)) {
        // Check if touch is on any interactive control inside scroll view
        CGPoint scrollPoint = [touch locationInView:self.scrollView];
        UIView *hitView = [self.scrollView hitTest:scrollPoint withEvent:nil];
        if ([hitView isKindOfClass:[UISlider class]] || 
            [hitView isKindOfClass:[UISwitch class]] || 
            [hitView isKindOfClass:[UIButton class]] ||
            [hitView isKindOfClass:[UITextField class]]) {
            return NO;
        }
    }
    
    return YES;
}

- (void)handlePan:(UIPanGestureRecognizer *)gesture {
    if (!self.canMove) return;
    
    CGPoint translation = [gesture translationInView:self.superview];
    if (gesture.state == UIGestureRecognizerStateBegan) self.lastLocation = self.center;
    self.center = CGPointMake(self.lastLocation.x + translation.x, self.lastLocation.y + translation.y);
}

- (void)closeButtonTapped:(UIButton *)sender { [self close]; }
- (void)addFeatureSwitch:(NSString *)title { [self addFeatureSwitch:title description:@"Custom feature" handler:nil]; }

- (void)addTab:(NSArray<NSString *> *)tabNames {
    if (!tabNames || tabNames.count == 0) return;
    
    CGFloat sidebarWidth = 110;
    CGFloat tabButtonHeight = 43;
    CGFloat tabSpacing = 3.6;
    CGFloat tabStartY = 8;
    
    // Get current number of tabs
    NSInteger startIndex = self.tabButtons.count;
    BOOL isFirstTab = (startIndex == 0);
    
    // Create new tab buttons
    for (NSInteger i = 0; i < tabNames.count; i++) {
        NSString *tabName = tabNames[i];
        NSInteger tabIndex = startIndex + i;
        
        UIButton *tabBtn = [UIButton buttonWithType:UIButtonTypeCustom];
        CGFloat yPosition = tabStartY + (tabIndex * (tabButtonHeight + tabSpacing));
        tabBtn.frame = CGRectMake(8, yPosition, sidebarWidth - 16, tabButtonHeight);
        [tabBtn setTitle:tabName forState:UIControlStateNormal];
        [tabBtn setTitleColor:[UIColor colorWithWhite:1.0 alpha:0.5] forState:UIControlStateNormal];
        [tabBtn setTitleColor:[UIColor whiteColor] forState:UIControlStateSelected];
        tabBtn.titleLabel.font = [UIFont systemFontOfSize:10 weight:UIFontWeightMedium];
        tabBtn.contentHorizontalAlignment = UIControlContentHorizontalAlignmentCenter;
        tabBtn.titleEdgeInsets = UIEdgeInsetsZero;
        tabBtn.tag = tabIndex;
        tabBtn.layer.cornerRadius = self.layer.cornerRadius * 0.3;
        // Tab unactive không có nền, chỉ tab active có nền
        tabBtn.backgroundColor = [UIColor clearColor];
        tabBtn.layer.borderWidth = 0;
        
        // Add bottom underline indicator for selected tab
        UIView *indicator = [[UIView alloc] initWithFrame:CGRectMake(0, tabButtonHeight - 2, 0, 2)];
        indicator.backgroundColor = self.accentColor;
        indicator.tag = 9999;
        indicator.hidden = YES;
        indicator.layer.cornerRadius = self.layer.cornerRadius * 0.1;
        [tabBtn addSubview:indicator];
        
        // Select first tab if this is the first tab being added
        if (isFirstTab && i == 0) {
            tabBtn.selected = YES;
            // Nền mờ theo màu accent của menu
            tabBtn.backgroundColor = [self.accentColor colorWithAlphaComponent:0.2];
            self.selectedTabIndex = 0;
            // Set initial width for first tab - centered
            CGSize textSize = [tabName sizeWithAttributes:@{NSFontAttributeName: tabBtn.titleLabel.font}];
            CGFloat buttonWidth = sidebarWidth - 16;
            CGFloat textX = (buttonWidth - textSize.width) / 2;
            indicator.frame = CGRectMake(textX, tabButtonHeight - 2, textSize.width, 2);
            indicator.hidden = NO;
        }
        
        [tabBtn addTarget:self action:@selector(tabButtonTapped:) forControlEvents:UIControlEventTouchUpInside];
        [self.tabContainerView addSubview:tabBtn];
        [self.tabButtons addObject:tabBtn];
    }
    
    // Update tabContainerView height and scroll view content size
    CGFloat totalHeight = tabStartY + (self.tabButtons.count * (tabButtonHeight + tabSpacing)) - tabSpacing;
    self.tabContainerView.frame = CGRectMake(0, 0, sidebarWidth, totalHeight);
    self.tabScrollView.contentSize = CGSizeMake(sidebarWidth, totalHeight);
}

- (void)customizeScrollIndicator {
    // Tùy chỉnh scroll indicator để có màu theo menu accent color
    // iOS tự động bo góc cho scroll indicator
    if ([[[UIDevice currentDevice] systemVersion] floatValue] >= 13.0) {
        // iOS 13+ có thể tùy chỉnh scroll indicator
        self.tabScrollView.indicatorStyle = UIScrollViewIndicatorStyleWhite;
    }
    
    // Sử dụng appearance proxy để set màu cho scroll indicator
    // Note: Scroll indicator màu sẽ tự động theo indicatorStyle
    // Bo góc được iOS tự động xử lý
}

- (void)scrollViewDidScroll:(UIScrollView *)scrollView {
    // Không cần update indicators nữa vì đã bỏ mũi tên
}

- (UIImage *)createSliderThumbImage {
    // Tạo thumb image nhỏ hơn một chút (kích thước mặc định ~23x23, giờ ~14x14)
    CGFloat thumbSize = 14.0;
    UIGraphicsBeginImageContextWithOptions(CGSizeMake(thumbSize, thumbSize), NO, 0.0);
    CGContextRef context = UIGraphicsGetCurrentContext();
    
    // Vẽ hình tròn màu trắng
    CGRect thumbRect = CGRectMake(0, 0, thumbSize, thumbSize);
    CGContextSetFillColorWithColor(context, [UIColor whiteColor].CGColor);
    CGContextFillEllipseInRect(context, thumbRect);
    
    // Thêm border nhẹ
    CGContextSetStrokeColorWithColor(context, [UIColor colorWithWhite:0.3 alpha:1.0].CGColor);
    CGContextSetLineWidth(context, 0.5);
    CGContextStrokeEllipseInRect(context, thumbRect);
    
    UIImage *image = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    return image;
}

@end