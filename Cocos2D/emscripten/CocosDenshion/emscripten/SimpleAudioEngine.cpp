/****************************************************************************
Copyright (c) 2013 Zynga Inc.
Copyright (c) 2010 cocos2d-x.org

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

// XXX: This is all just a bit of a hack. SDL uses channels as its underlying
// abstraction, whilst CocosDenshion deals in individual effects. Consequently
// we can't set start/stop, because to SDL, effects (chunks) are just opaque
// blocks of data that get scheduled on channels. To workaround this, we assign
// each sound to a channel, but since there are only 32 channels, we use the
// modulus of the sound's address (which on Emscripten is just an incrementing
// integer) to decide which channel.
//
// A more rigorous implementation would have logic to store the state of
// channels and restore it as necessary. This should probably just be
// considered a toy for now, but it will probably prove sufficient for many
// use-cases. Recall also that Emscripten undoes this abstraction on the other
// side because it is using HTML5 audio objects as its underlying primitive!

#include <map>
#include <string>
#include <stdio.h>
#include <unistd.h>

#include "SimpleAudioEngine.h"
#include "cocos2d.h"

extern "C" {
    // Methods implemented in SimpleAudioEngine.js
    void SimpleAudioEngine_init();

    void SimpleAudioEngine_rewindBackgroundMusic();
    void SimpleAudioEngine_preloadEffect(const char *);
    int SimpleAudioEngine_playEffect(const char *, int);
    void SimpleAudioEngine_stopEffect(int);
    void SimpleAudioEngine_setEffectsVolume(int);
    void SimpleAudioEngine_pauseEffect(int);
    void SimpleAudioEngine_resumeEffect(int);
    void SimpleAudioEngine_pauseAllEffects();
    void SimpleAudioEngine_resumeAllEffects();
    void SimpleAudioEngine_stopAllEffects();

    void SimpleAudioEngine_preloadBackgroundMusic(const char *);
    int SimpleAudioEngine_isBackgroundMusicPlaying();
    void SimpleAudioEngine_playBackgroundMusic(const char *, int);
    void SimpleAudioEngine_pauseBackgroundMusic();
    void SimpleAudioEngine_resumeBackgroundMusic();
    void SimpleAudioEngine_stopBackgroundMusic();
    void SimpleAudioEngine_setBackgroundMusicVolume(int);

    void SimpleAudioEngine_end();
};

USING_NS_CC;

using namespace std;

namespace CocosDenshion
{
    float s_effectsVolume = 1.0;
    float s_backgroundVolume = 1.0;
    static SimpleAudioEngine  *s_engine = 0;

    static void stopBackground(bool bReleaseData)
    {
        SimpleAudioEngine_stopBackgroundMusic();
    }

    SimpleAudioEngine::SimpleAudioEngine()
    {
        SimpleAudioEngine_init();
    }

    SimpleAudioEngine::~SimpleAudioEngine()
    {
    }

    SimpleAudioEngine* SimpleAudioEngine::sharedEngine()
    {
        if (!s_engine)
            s_engine = new SimpleAudioEngine();
        
        return s_engine;
    }

    void SimpleAudioEngine::end()
    {
        SimpleAudioEngine_end();
    }

    //
    // background audio
    //
    void SimpleAudioEngine::preloadBackgroundMusic(const char* pszFilePath)
    {
        SimpleAudioEngine_preloadBackgroundMusic(pszFilePath);
    }

    void SimpleAudioEngine::playBackgroundMusic(const char* pszFilePath, bool bLoop)
    {
        SimpleAudioEngine_playBackgroundMusic(pszFilePath, bLoop);
    }

    void SimpleAudioEngine::stopBackgroundMusic(bool bReleaseData)
    {
        SimpleAudioEngine_stopBackgroundMusic();
    }

    void SimpleAudioEngine::pauseBackgroundMusic()
    {
        SimpleAudioEngine_pauseBackgroundMusic();
    }

    void SimpleAudioEngine::resumeBackgroundMusic()
    {
        SimpleAudioEngine_resumeBackgroundMusic();
    } 

    void SimpleAudioEngine::rewindBackgroundMusic()
    {
        SimpleAudioEngine_rewindBackgroundMusic();
    }

    bool SimpleAudioEngine::willPlayBackgroundMusic()
    {
        return true;
    }

    bool SimpleAudioEngine::isBackgroundMusicPlaying()
    {
        return SimpleAudioEngine_isBackgroundMusicPlaying();
    }

    float SimpleAudioEngine::getBackgroundMusicVolume()
    {
        return s_backgroundVolume;
    }

    void SimpleAudioEngine::setBackgroundMusicVolume(float volume)
    {
        // Ensure volume is between 0.0 and 1.0.
        volume = volume > 1.0 ? 1.0 : volume;
        volume = volume < 0.0 ? 0.0 : volume;

        SimpleAudioEngine_setBackgroundMusicVolume((int) (volume * 100));
        s_backgroundVolume = volume;
    }

    float SimpleAudioEngine::getEffectsVolume()
    {
        return s_effectsVolume;
    }

    void SimpleAudioEngine::setEffectsVolume(float volume)
    {
        volume = volume > 1.0 ? 1.0 : volume;
        volume = volume < 0.0 ? 0.0 : volume;
        SimpleAudioEngine_setEffectsVolume((int) (volume * 100));
        s_effectsVolume = volume;
    }

    unsigned int SimpleAudioEngine::playEffect(const char* pszFilePath, bool bLoop)
    {
        return SimpleAudioEngine_playEffect(pszFilePath, bLoop);
    }

    void SimpleAudioEngine::stopEffect(unsigned int nSoundId)
    {
        SimpleAudioEngine_stopEffect(nSoundId);
    }

    void SimpleAudioEngine::preloadEffect(const char* pszFilePath)
    {
        SimpleAudioEngine_preloadEffect(pszFilePath);
    }

    void SimpleAudioEngine::unloadEffect(const char* pszFilePath)
    {
        // This doesn't make as much sense here. Just ignore.
    }

    void SimpleAudioEngine::pauseEffect(unsigned int nSoundId)
    {
        SimpleAudioEngine_pauseEffect(nSoundId);
    }

    void SimpleAudioEngine::pauseAllEffects()
    {
        SimpleAudioEngine_pauseAllEffects();
    }

    void SimpleAudioEngine::resumeEffect(unsigned int nSoundId)
    {
        SimpleAudioEngine_resumeEffect(nSoundId);
    }

    void SimpleAudioEngine::resumeAllEffects()
    {
        SimpleAudioEngine_resumeAllEffects();
    }

    void SimpleAudioEngine::stopAllEffects()
    {
        SimpleAudioEngine_stopAllEffects();
    }

}
