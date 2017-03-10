/****************************************************************************
Copyright (c) 2013 Zynga Inc.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

// SimpleAudioEngine implemented here as a thin wrapper over SoundManager 2,
// which will use HTML5 audio where available, and fallback to Flash where
// necessary. Gives a much more reliable audio experience without needing to
// sacrifice clients that do not have Flash available.

var LibrarySimpleAudioEngine = {
    SimpleAudioEngine_init: function()
    {
        Module.CocosDensionState = {
            effectMap: {},
            effectVolume: 100,

            musicMap: {},
            musicVolume: 100,
            curBackgroundMusicId: null
        };

        // Audio slows Android down (drops a ton of frames), and it's flakey
        // anyway. Disable audio on Android for now. Can revisit in the future
        // if this situation improves.
        var ua = navigator.userAgent.toLowerCase();
        var isAndroid = ua.indexOf("android") > -1;
        if(!isAndroid)
        {
            soundManager.setup({
                url: './',
                flashVersion: 9
            });
        }
    },

    SimpleAudioEngine_playEffect: function(filenameP, loop)
    {
        // XXX: Note that effects, unlike background music, are indexed by an
        // ID number in CocosDenshion, hence we need to maintain a mapping of
        // generated IDs from their filenames in order for this all to
        // function.
        var filename = Pointer_stringify(filenameP);
        var effectNum = null;
        var sound = null;
        var loops = loop ? 10000 : 1;

        if(Module.CocosDensionState.effectMap[filename] === undefined)
        {
            effectNum = Object.keys(Module.CocosDensionState.effectMap).length;
            Module.CocosDensionState.effectMap[filename] = effectNum;
            soundManager.onready(function() {
                sound = soundManager.createSound({
                    id: 'SimpleAudioEngine_Effect_' + effectNum,
                    url: filename,
                    onload: function() {
                        this.play({
                            loops: loops,
                            volume: Module.CocosDensionState.effectVolume
                        });
                    }
                });
                sound.load();
            });
        }
        else
        {
            effectNum = Module.CocosDensionState.effectMap[filename];
            soundManager.onready(function() {
                sound = soundManager.getSoundById('SimpleAudioEngine_Effect_' + effectNum);
                sound.play({
                    loops: loops,
                    volume: Module.CocosDensionState.effectVolume
                });
            });
        }

        return effectNum;
    },

    SimpleAudioEngine_preloadEffect: function(filenameP)
    {
        var filename = Pointer_stringify(filenameP);
        soundManager.onready(function() {
            if(Module.CocosDensionState.effectMap[filename] === undefined)
            {
                effectNum = Object.keys(Module.CocosDensionState.effectMap).length;
                Module.CocosDensionState.effectMap[filename] = effectNum;
                sound = soundManager.createSound({
                    id: 'SimpleAudioEngine_Effect_' + effectNum,
                    url: filename
                });
                sound.load();
            }
        });
    },

    SimpleAudioEngine_end: function()
    {
        soundManager.onready(function() {
            soundManager.stopAll();
        });
    },

    SimpleAudioEngine_isBackgroundMusicPlaying: function()
    {
        if(Module.CocosDensionState.curBackgroundMusicId == null)
        {
            return 0;
        }

        try
        {
            var sound = soundManager.getSoundById(Module.CocosDensionState.curBackgroundMusicId);
            if(sound === null)
            {
                return 0;
            }

            return sound.playState;
        }
        catch(e)
        {
            // If an exception is thrown reading the state of the audio, it's
            // most likely because soundmanager hasn't yet initialized, so we
            // can safely assume that no sound is playing.
            return 0;
        }
    },

    SimpleAudioEngine_stopBackgroundMusic: function()
    {
        soundManager.onready(function() {
            for(var soundId in soundManager.sounds)
            {
                if(soundManager.sounds.hasOwnProperty(soundId))
                {
                    if(soundId.indexOf('SimpleAudioEngine_Music_') == 0)
                    {
                        soundManager.stop(soundId);
                    }
                }
            }
            Module.CocosDensionState.curBackgroundMusicId = null;
        });
    },

    SimpleAudioEngine_setBackgroundMusicVolume: function(volume)
    {
        if(volume > 100) volume = 100;
        if(volume < 0) volume = 0;

        soundManager.onready(function() {
            Module.CocosDensionState.musicVolume = volume;
            
            for(var filename in Module.CocosDensionState.musicMap)
            {
                if(Module.CocosDensionState.musicMap.hasOwnProperty(filename))
                {
                    var music = Module.CocosDensionState.musicMap[filename];
                    music.setVolume(volume);
                }
            }
        });
    },

    SimpleAudioEngine_playBackgroundMusic__deps: [ 'SimpleAudioEngine_stopBackgroundMusic' ],
    SimpleAudioEngine_playBackgroundMusic: function(filenameP, loop)
    {
        var filename = Pointer_stringify(filenameP);
        var soundId = 'SimpleAudioEngine_Music_' + filename;
        var loops = loop ? 10000 : 1;

        _SimpleAudioEngine_stopBackgroundMusic();
        soundManager.onready(function() {
            var sound = soundManager.getSoundById(soundId);
            if(!sound)
            {
                sound = soundManager.createSound({
                    id: soundId,
                    url: filename,
                    onload: function() {
                        this.play({
                            loops: loops,
                            volume: Module.CocosDensionState.musicVolume
                        });
                    }
                });
                sound.load();
            }
            else
            {
                sound.play({
                    loops: loops,
                    volume: Module.CocosDensionState.musicVolume
                });
            }
            Module.CocosDensionState.curBackgroundMusicId = soundId;
            Module.CocosDensionState.musicMap[filename] = sound;
        });
    },

    SimpleAudioEngine_preloadBackgroundMusic: function(filenameP)
    {
        var filename = Pointer_stringify(filenameP);
        var soundId = 'SimpleAudioEngine_Music_' + filename;

        soundManager.onready(function() {
            var sound = soundManager.getSoundById(soundId);
            if(!sound)
            {
                sound = soundManager.createSound({
                    id: soundId,
                    url: filename
                });
                sound.load();
                Module.CocosDensionState.musicMap[filename] = sound;
            }
        });
    },

    SimpleAudioEngine_pauseBackgroundMusic: function()
    {
        soundManager.onready(function() {
            if(Module.CocosDensionState.curBackgroundMusicId)
            {
                soundManager.pause(Module.CocosDensionState.curBackgroundMusicId);
            }
        });
    },

    SimpleAudioEngine_rewindBackgroundMusic: function()
    {
        soundManager.onready(function() {
            if(Module.CocosDensionState.curBackgroundMusicId == null)
            {
                return;
            }
            var sound = soundManager.getSoundById(Module.CocosDensionState.curBackgroundMusicId);
            if(sound)
            {
                sound.setPosition(0);
            }
        });
    },

    SimpleAudioEngine_resumeBackgroundMusic: function()
    {
        soundManager.onready(function() {
            if(Module.CocosDensionState.curBackgroundMusicId)
            {
                soundManager.resume(Module.CocosDensionState.curBackgroundMusicId);
            }
        });
    },

    SimpleAudioEngine_stopEffect: function(effectId)
    {
        soundManager.onready(function() {
            soundManager.stop('SimpleAudioEngine_Effect_' + effectId);
        });
    },

    SimpleAudioEngine_setEffectsVolume: function(volume)
    {
        if(volume > 100) volume = 100;
        if(volume < 0) volume = 0;

        soundManager.onready(function() {
            Module.CocosDensionState.effectVolume = volume;

            for(var filename in Module.CocosDensionState.effectMap)
            {
                if(Module.CocosDensionState.effectMap.hasOwnProperty(filename))
                {
                    effectNum = Module.CocosDensionState.effectMap[filename];
                    soundManager.setVolume('SimpleAudioEngine_Effect_' + effectNum, volume);
                }
            }
        });
    },

    SimpleAudioEngine_pauseEffect: function(effectId)
    {
        soundManager.onready(function() {
            soundManager.pause('SimpleAudioEngine_Effect_' + effectId);
        });
    },

    SimpleAudioEngine_resumeEffect: function(effectId)
    {
        soundManager.onready(function() {
            soundManager.resume('SimpleAudioEngine_Effect_' + effectId);
        });
    },

    SimpleAudioEngine_pauseAllEffects: function()
    {
        soundManager.onready(function() {
            for(var filename in Module.CocosDensionState.effectMap)
            {
                if(Module.CocosDensionState.effectMap.hasOwnProperty(filename))
                {
                    effectNum = Module.CocosDensionState.effectMap[filename];
                    soundManager.pause('SimpleAudioEngine_Effect_' + effectNum);
                }
            }
        });
    },

    SimpleAudioEngine_resumeAllEffects: function()
    {
        soundManager.onready(function() {
            for(var filename in Module.CocosDensionState.effectMap)
            {
                if(Module.CocosDensionState.effectMap.hasOwnProperty(filename))
                {
                    effectNum = Module.CocosDensionState.effectMap[filename];
                    soundManager.resume('SimpleAudioEngine_Effect_' + effectNum);
                }
            }
        });
    },

    SimpleAudioEngine_stopAllEffects: function()
    {
        soundManager.onready(function() {
            for(var filename in Module.CocosDensionState.effectMap)
            {
                if(Module.CocosDensionState.effectMap.hasOwnProperty(filename))
                {
                    effectNum = Module.CocosDensionState.effectMap[filename];
                    soundManager.stop('SimpleAudioEngine_Effect_' + effectNum);
                }
            }
        });
    }
};

mergeInto(LibraryManager.library, LibrarySimpleAudioEngine);

