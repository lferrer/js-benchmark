EXECUTABLE = TestCpp

SOURCES = ../Classes/AccelerometerTest/AccelerometerTest.cpp \
	../Classes/ActionManagerTest/ActionManagerTest.cpp \
	../Classes/ActionsEaseTest/ActionsEaseTest.cpp \
	../Classes/ActionsProgressTest/ActionsProgressTest.cpp \
	../Classes/ActionsTest/ActionsTest.cpp \
	../Classes/Box2DTest/Box2dTest.cpp \
	../Classes/Box2DTestBed/Box2dView.cpp \
	../Classes/Box2DTestBed/GLES-Render.cpp \
	../Classes/Box2DTestBed/Test.cpp \
	../Classes/Box2DTestBed/TestEntries.cpp \
	../Classes/BugsTest/Bug-1159.cpp \
	../Classes/BugsTest/Bug-1174.cpp \
	../Classes/BugsTest/Bug-350.cpp \
	../Classes/BugsTest/Bug-422.cpp \
	../Classes/BugsTest/Bug-458/Bug-458.cpp \
	../Classes/BugsTest/Bug-458/QuestionContainerSprite.cpp \
	../Classes/BugsTest/Bug-624.cpp \
	../Classes/BugsTest/Bug-886.cpp \
	../Classes/BugsTest/Bug-899.cpp \
	../Classes/BugsTest/Bug-914.cpp \
	../Classes/BugsTest/BugsTest.cpp \
	../Classes/ChipmunkTest/ChipmunkTest.cpp \
	../Classes/ClickAndMoveTest/ClickAndMoveTest.cpp \
	../Classes/ClippingNodeTest/ClippingNodeTest.cpp \
	../Classes/CocosDenshionTest/CocosDenshionTest.cpp \
	../Classes/CurrentLanguageTest/CurrentLanguageTest.cpp \
	../Classes/DrawPrimitivesTest/DrawPrimitivesTest.cpp \
	../Classes/EffectsAdvancedTest/EffectsAdvancedTest.cpp \
	../Classes/EffectsTest/EffectsTest.cpp \
	../Classes/ExtensionsTest/CocosBuilderTest/ButtonTest/ButtonTestLayer.cpp \
	../Classes/ExtensionsTest/CocosBuilderTest/CocosBuilderTest.cpp \
	../Classes/ExtensionsTest/CocosBuilderTest/HelloCocosBuilder/HelloCocosBuilderLayer.cpp \
	../Classes/ExtensionsTest/CocosBuilderTest/AnimationsTest/AnimationsTestLayer.cpp \
	../Classes/ExtensionsTest/CocosBuilderTest/MenuTest/MenuTestLayer.cpp \
	../Classes/ExtensionsTest/CocosBuilderTest/TestHeader/TestHeaderLayer.cpp \
	../Classes/ExtensionsTest/CocosBuilderTest/TimelineCallbackTest/TimelineCallbackTestLayer.cpp \
	../Classes/ExtensionsTest/ControlExtensionTest/CCControlButtonTest/CCControlButtonTest.cpp \
	../Classes/ExtensionsTest/ControlExtensionTest/CCControlColourPicker/CCControlColourPickerTest.cpp \
	../Classes/ExtensionsTest/ControlExtensionTest/CCControlScene.cpp \
	../Classes/ExtensionsTest/ControlExtensionTest/CCControlSceneManager.cpp \
	../Classes/ExtensionsTest/ControlExtensionTest/CCControlSliderTest/CCControlSliderTest.cpp \
	../Classes/ExtensionsTest/ControlExtensionTest/CCControlSwitchTest/CCControlSwitchTest.cpp \
	../Classes/ExtensionsTest/ControlExtensionTest/CCControlPotentiometerTest/CCControlPotentiometerTest.cpp \
	../Classes/ExtensionsTest/ControlExtensionTest/CCControlStepperTest/CCControlStepperTest.cpp \
	../Classes/ExtensionsTest/TableViewTest/TableViewTestScene.cpp \
	../Classes/ExtensionsTest/TableViewTest/CustomTableViewCell.cpp \
	../Classes/ExtensionsTest/ExtensionsTest.cpp \
	../Classes/ExtensionsTest/NotificationCenterTest/NotificationCenterTest.cpp \
	../Classes/ExtensionsTest/NetworkTest/HttpClientTest.cpp \
    ../Classes/ExtensionsTest/ComponentsTest/ComponentsTestScene.cpp \
    ../Classes/ExtensionsTest/ComponentsTest/EnemyController.cpp \
    ../Classes/ExtensionsTest/ComponentsTest/GameOverScene.cpp \
    ../Classes/ExtensionsTest/ComponentsTest/PlayerController.cpp \
    ../Classes/ExtensionsTest/ComponentsTest/ProjectileController.cpp \
    ../Classes/ExtensionsTest/ComponentsTest/SceneController.cpp \
	../Classes/ExtensionsTest/ArmatureTest/ArmatureScene.cpp \
	../Classes/FontTest/FontTest.cpp \
	../Classes/IntervalTest/IntervalTest.cpp \
	../Classes/KeypadTest/KeypadTest.cpp \
	../Classes/LabelTest/LabelTest.cpp \
	../Classes/LayerTest/LayerTest.cpp \
	../Classes/MenuTest/MenuTest.cpp \
	../Classes/MotionStreakTest/MotionStreakTest.cpp \
	../Classes/MutiTouchTest/MutiTouchTest.cpp \
	../Classes/NodeTest/NodeTest.cpp \
	../Classes/ParallaxTest/ParallaxTest.cpp \
	../Classes/ParticleTest/ParticleTest.cpp \
	../Classes/PerformanceTest/PerformanceNodeChildrenTest.cpp \
	../Classes/PerformanceTest/PerformanceParticleTest.cpp \
	../Classes/PerformanceTest/PerformanceSpriteTest.cpp \
	../Classes/PerformanceTest/PerformanceTest.cpp \
	../Classes/PerformanceTest/PerformanceTextureTest.cpp \
	../Classes/PerformanceTest/PerformanceTouchesTest.cpp \
	../Classes/RenderTextureTest/RenderTextureTest.cpp \
	../Classes/RotateWorldTest/RotateWorldTest.cpp \
	../Classes/SceneTest/SceneTest.cpp \
	../Classes/SchedulerTest/SchedulerTest.cpp \
	../Classes/ShaderTest/ShaderTest.cpp \
	../Classes/SpriteTest/SpriteTest.cpp \
	../Classes/TextInputTest/TextInputTest.cpp \
	../Classes/Texture2dTest/Texture2dTest.cpp \
	../Classes/TextureCacheTest/TextureCacheTest.cpp \
	../Classes/TileMapTest/TileMapTest.cpp \
	../Classes/TouchesTest/Ball.cpp \
	../Classes/TouchesTest/Paddle.cpp \
	../Classes/TouchesTest/TouchesTest.cpp \
	../Classes/TransitionsTest/TransitionsTest.cpp \
	../Classes/UserDefaultTest/UserDefaultTest.cpp \
	../Classes/ZwoptexTest/ZwoptexTest.cpp \
	../Classes/FileUtilsTest/FileUtilsTest.cpp \
	../Classes/DataVisitorTest/DataVisitorTest.cpp \
	../Classes/ConfigurationTest/ConfigurationTest.cpp \
	../Classes/controller.cpp \
	../Classes/testBasic.cpp \
	../Classes/AppDelegate.cpp \
	../Classes/VisibleRect.cpp \
	main.cpp

RESOURCE_PATH = ../Resources

# We process PNGs separately to the rest of the resources so that we can do
# compile-time premultiply on them.
RESOURCES := $(shell (cd $(RESOURCE_PATH); find . -type f ! -name '* *' ! -name '*.mp3' ! -name '*.png'))
PNGS := $(shell (cd $(RESOURCE_PATH); find . -type f ! -name '* *' -name '*.png'))

SHAREDLIBS = -lcocos2d -lcocosdenshion
COCOS_LIBS = $(LIB_DIR)/libcocos2d.so $(LIB_DIR)/libcocosdenshion.so

include ../../../../cocos2dx/proj.emscripten/cocos2dx.mk

INCLUDES +=  -I../ \
    -I../../ \
    -I../Classes \
    -I$(COCOS_ROOT)/CocosDenshion/include \
    -I$(COCOS_ROOT)/extensions/ \
    -I$(COCOS_ROOT)/external/ \
    -I$(COCOS_ROOT)/external/chipmunk/include/chipmunk

STATICLIBS += \
    $(LIB_DIR)/libextension.a \
    $(LIB_DIR)/libbox2d.a \
    $(LIB_DIR)/libchipmunk.a

DEFINES += -DCC_ENABLE_CHIPMUNK_INTEGRATION=1

####### Build rules
$(TARGET).js: $(OBJECTS) $(COCOS_LIBS) $(CORE_MAKEFILE_LIST)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@ $(SHAREDLIBS) $(LIBS)

$(OBJ_DIR)/Makefile.images: $(patsubst %,$(RESOURCE_PATH)/%,$(PNGS))
	# This snippet generates a Makefile that will transcode images from normal,
	# 8-bit-per-channel PNG, to pre-multiplied, max zlib compressed PNG.
	# Pre-multiply saves a lot of runtime compute work. Note, this is also a
	# great spot to apply compression to the images if that is desired.  Not
	# that the "-quality" flag is interpreted differently for PNG files than
	# JPEGs.
	# 
	# The approach of emitting a Makefile and then running it is used because
	# this transcoding is actually moderately expensive (so can't just do a for
	# loop in the rule as it would invoke the transcoding unnecessarily), and
	# dynamically adding rules to a running Make for this is more complexity
	# than I have time to deal with. This actually works pretty well.
	(/bin/echo -n "all: "; \
	imgs=`find $(RESOURCE_PATH) -iname "*.png"`; \
	for i in $$imgs; do \
		out=$(BIN_DIR)/`echo $$i | perl -pe "s@$(RESOURCE_PATH)@/@"`; \
		/bin/echo -n "$$out "; \
	done; \
	echo; \
	for i in $$imgs; do \
		out=$(BIN_DIR)/`echo $$i | perl -pe "s@$(RESOURCE_PATH)@/@"`; \
		(echo $$out: $$i; \
		echo "\t"mkdir -p `dirname $$out`; \
		echo "\t"echo $$out; \
		echo "\t"convert \
	   		-size \`identify -format \'%[fx:w]x%[fx:h]\' $(RESOURCE_PATH)/$$i\` xc:black \
			-alpha on \
			-draw \"image over 0,0 0,0 \'$(RESOURCE_PATH)/$$i\'\" \
			-draw \"image copy-opacity 0,0 0,0 \'$(RESOURCE_PATH)/$$i\'\" \
			-depth 8 -quality 95 \'$$out\'; \
		echo); \
	done) > $(OBJ_DIR)/Makefile.images

$(TARGET).data: $(CORE_MAKEFILE_LIST) $(OBJ_DIR)/Makefile.images $(patsubst %,$(RESOURCE_PATH)/%,$(RESOURCES)) $(patsubst %,$(RESOURCE_PATH)/%,$(PNGS))
	@mkdir -p $(@D)
	$(eval RESTMP := $(shell mktemp -d /tmp/cocos-emscripten.XXXXXX))
	# Always need a font path, so ensure that it is created first.
	mkdir -p $(RESTMP)/fonts
	(cd $(RESOURCE_PATH) && tar cf - $(RESOURCES)) | (cd $(RESTMP) && tar xvf -)
	
	# Now transcode PNG files into the temp output directory
	(for i in $(PNGS); do \
		mkdir -p `dirname $(RESTMP)/$$i`; \
		(echo convert \
			-size \`identify -format \'%[fx:w]x%[fx:h]\' $(RESOURCE_PATH)/$$i\` xc:black \
			-alpha on \
			-draw \"image over 0,0 0,0 \'$(RESOURCE_PATH)/$$i\'\" \
			-draw \"image copy-opacity 0,0 0,0 \'$(RESOURCE_PATH)/$$i\'\" \
			-depth 8 -quality 95 \'$(RESTMP)/$$i\') | sh -e; \
	done)
		
	(cd $(RESTMP); python $(PACKAGER) $(EXECUTABLE).data $(patsubst %,--preload %,$(RESOURCES)) $(patsubst %,--preload %,$(PNGS)) --preload fonts --pre-run > $(EXECUTABLE).data.js)
	mv $(RESTMP)/$(EXECUTABLE).data $@
	mv $(RESTMP)/$(EXECUTABLE).data.js $@.js
	
	# Copy all assets except PNG files.
	rsync --exclude="*.png" --exclude="*.PNG" -rv $(RESOURCE_PATH)/ $(BIN_DIR)/
	make $(MAKEFLAGS) -f $(OBJ_DIR)/Makefile.images all
	rm -rf $(RESTMP)

$(BIN_DIR)/index.html: index.html $(CORE_MAKEFILE_LIST)
	@mkdir -p $(@D)
	cp index.html $(@D)

####### Compile
$(OBJ_DIR)/%.o: ../%.cpp $(CORE_MAKEFILE_LIST)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(DEFINES) -c $< -o $@

$(OBJ_DIR)/%.o: %.cpp $(CORE_MAKEFILE_LIST)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(DEFINES) -c $< -o $@

$(OBJ_DIR)/%.o: %.c $(CORE_MAKEFILE_LIST)
	@mkdir -p $(@D)
	$(CC) $(CCFLAGS) $(INCLUDES) $(DEFINES) -c $< -o $@
