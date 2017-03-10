/****************************************************************************
Copyright (c) 2013      Zynga Inc.

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


// Non-namespaced facade over underlying objects. See above methods for
// documentation. These methods serve two purposes:
//
// 1. convert types to/from JS equivalents.
// 2. expose a "C-like" interface, such that the symbols are easy to find
//    for calling code (Emscripten provides no easy way to access nested
//    dictionaries).
//
// Note that the leading '_' is because Emscripten prefixes all function
// calls like this. Since this code is being injected at runtime,
// Emscripten's compiler doesn't have a chance to add it for us.


var LibraryCocosHelper = {
    $cocos2dx__deps: [ '_CCTextureCacheEmscripten_preMultiplyImageRegion' ],
    $cocos2dx: {
        objects: {},
        classes: {
            // AsyncOperationQueue -- simple worker queue. Note that all functions
            // passed in should effectively be "static", and have all requisite
            // information contained in their args object.
            AsyncOperationQueue: function()
            {
                this.ops = [];
                this.timeoutId = null;

                // Target using 2/3 of available CPU for texture loading.
                this.sliceBudget = 8;
                this.sliceInterval = 4;

                /**
                 * If there is a valid scheduled queue consumer, cancel it. Won't cancel
                 * currently executing operations (as this is not multi-threaded), but will
                 * prevent future execution from happening until it is re-scheduled.
                 */
                this.unschedule = function(interval)
                {
                    if(typeof this.timeoutId == "number")
                    {
                        clearTimeout(this.timeoutId);
                        this.timeoutId = null;
                    }
                };

                /**
                 * Schedule the queue-processor to run @interval ms in the future.
                 */
                this.schedule = function(interval)
                {
                    this.unschedule();

                    var that = this;
                    var o = function() {
                        that.run();
                    };

                    this.timeoutId = setTimeout(o, interval);
                };

                /**
                 * Enqueue a funtcion @fn with arguments @args to run asynchronously at
                 * some point in the future, then return control to the caller. Operations
                 * are guaranteed to be run in the order in which they are enqueued, but no
                 * guarantee is given about when or in what context they will be executed.
                 */
                this.enqueue = function(fn, args)
                {
                    var op = {
                        fn: fn,
                        args: args
                    };
                    this.ops.push(op);
                    this.schedule(this.sliceInterval);
                };

                /**
                 * Start running the consumer "thread". Will consume operations from the
                 * queue until such time as either the queue is empty, or it has exceeded
                 * its time budget. If the latter case (budget exceeded), it will
                 * reschedule itself to run again later and finish draining the queue.
                 *
                 * Rescheduling logic still in flux, but the objective is to try to
                 * schedule it to run at some point after the next frame has been
                 * requested, whilst still maintaining maximum bandwidth.
                 */
                this.run = function()
                {
                    if(this.ops.length === 0)
                    {
                        return;
                    }

                    var start = +new Date();
                    var end = +new Date();
                    while(this.ops.length && ((end - start) < this.sliceBudget))
                    {
                        var op = this.ops.shift();
                        op.fn(op.args);
                        end = +new Date();
                    }

                    if(this.ops.length)
                    {
                        this.schedule(this.sliceInterval);
                    }
                };

                /**
                 * Unschedule any remaining operations in the queue.
                 */
                this.shutdown = function()
                {
                    this.unschedule();
                    this.ops = [];
                };
            },

            /**
             * Construct a new AsyncImageLoader object. @cxxTextureCache should be a
             * pointer to a valid CCTextureCacheEmscripten object in C++.
             */
            AsyncImageLoader: function(cxxTextureCache)
            {
                // NOTE: cocos-emscripten now expects all alpha images to be
                // pre-multiplied at COMPILE TIME. This is a breaking change vs
                // previously published versions. Samples have been updated to
                // show how to do this. If you have legacy code that relies on
                // the runtime pre-multiplying behavior, then switch this to
                // true to get the old behavior.
                this.runtimePremultiply = false;

                this.cxxTextureCache = cxxTextureCache;
                this.operationQueue = new cocos2dx.classes.AsyncOperationQueue();

                // Note: Chrome renders using hardware for any offscreen canvas
                // > 65536px (256x256, for instance). This results in a
                // glReadPixels call to fetch the image data back in
                // getImageData(), which is extremely slow. Consequently our
                // stategy here is to use small canvases which are safely under
                // this threshold so that all the image data stays on host
                // memory. This results in a ~10X performance improvement on
                // image loads.
                this.regionSize = 128;

                /**
                 * "Private" and "static" method to copy an input image to a given
                 * rectangle in the output image. Used by @loadImage.
                 */
                this._blitAndPremultipyRegion = function(args)
                {
                    // Calculate width and height for this region such that we
                    // do not run off the edge of the image.
                    var rw = Math.min(args.i + this.regionSize, args.w) - args.i;
                    var rh = Math.min(args.j + this.regionSize, args.h) - args.j;

                    var canvas = document.createElement('canvas');
                    canvas.width = rw;
                    canvas.height = rh;

                    var ctx = canvas.getContext('2d');
                    ctx.drawImage(args.img, args.i, args.j, rw, rh, 0, 0, rw, rh);

                    if(this.runtimePremultiply)
                    {
                        var inImgData = _malloc(rw * rh * 4);
                        var imgData = ctx.getImageData(0, 0, rw, rh);
                        Module.HEAP8.set(imgData.data, inImgData);

                        // Call into C++ code in CCTextureCacheEmscripten.cpp to do the actual
                        // copy and pre-multiply.
                        _CCTextureCacheEmscripten_preMultiplyImageRegion(
                            inImgData, rw, rh,
                            args.outImgData, args.w, args.h,
                            args.i, args.j);

                        _free(inImgData);
                    }
                    else
                    {
                        // When not doing runtime pre-multiply, we just want to
                        // copy the image data into the appropriate rectangle on
                        // the target buffer. We use ArrayBuffer.set calls for each
                        // row so as to make this as painless as possible.
                        for(var j = 0; j < rh; j++)
                        {
                            // Still not totally satisfied that this is a win.
                            // getImageData doesn't show up on profiles, nor
                            // does the premultiply code, but it's also not
                            // going any faster.
                            var imgData = ctx.getImageData(0, j, rw, 1);
                            var outOffset = 4 * ((j + args.j) * args.w + args.i);
                            Module.HEAP8.set(imgData.data, args.outImgData + outOffset);
                        }
                    }
                };

                /**
                 * Enqueue the image at @path (interpreted as a URL) to be asynchronously
                 * loaded, then call the cocos2dx callback specified in @asyncData. Image
                 * fetch happens asynchronously using browser asset management code, and
                 * the work of copying pixel data to a buffer (for use by cocos2dx) is
                 * spread out over several frames such that disruptions to frame rate are
                 * minimized.
                 */
                this.loadImage = function(path, asyncData)
                {
                    var img = new Image();
                    var that = this;

                    img.onload = function()
                    {
                        // This delicate tango is so that we can use WebGL's
                        // texture loading methods directly, but maintain
                        // Emscripten's GL emulation layer's state with as
                        // little sleaze as possible. The objective is to use
                        // the fastest path to loading texture data possible,
                        // but to still be able to take advantage of the OpenGL
                        // emulation.
                        var p_texName = _malloc(4); // sizeof(GLuint) == 4.
                        _glGenTextures(1, p_texName);
                        var textureId = getValue(p_texName, 'i32');
                        _free(p_texName);

                        var gl = Module.ctx;
                        _glActiveTexture(gl.TEXTURE0);
                        _glBindTexture(gl.TEXTURE_2D, textureId);

                        _glTexParameteri( gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR );
                        _glTexParameteri( gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR );
                        _glTexParameteri( gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE );
                        _glTexParameteri( gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE );

                        gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, img);

                        _CCTextureCacheEmscripten_addImageAsyncCallBack(that.cxxTextureCache, asyncData, textureId, img.width, img.height);
                    };

                    img.onerror = function()
                    {
                        console.log("Error loading '" + path + "'");
                    };

                    img.src = path;
                };

                /**
                 * Shutdown this image loader object. Used by destructor in cocos2dx.
                 */
                this.shutdown = function()
                {
                    this.operationQueue.shutdown();
                };
            }
        }
    },

    /*
     * Construct a new AsyncImageLoader object. Held as a singleton, referred
     * to in other wrapper methods here.
     * @deps__ignored is ignored, but is accepted so that we can communicate to
     * the compiler what functions we need to ensure are ready by the time this
     * class gets instantiated.
     */
    cocos2dx_newAsyncImageLoader: function(cxxTextureCache, deps__ignored)
    {
        cocos2dx.objects.asyncImageLoader = new cocos2dx.classes.AsyncImageLoader(cxxTextureCache);
    },

    /**
     * Shutdown the current image loader object. Used by the cocos2dx
     * destructor method.
     */
    cocos2dx_shutdownAsyncImageLoader: function()
    {
        cocos2dx.objects.asyncImageLoader.shutdown();
        cocos2dx.objects.asyncImageLoader = null;
    },

    /**
     * Load a new image asynchronously.
     */
    cocos2dx_asyncImageLoader_LoadImage: function(path, asyncData)
    {
        var opArgs = {
            path: Pointer_stringify(path),
            data: asyncData
        };
        var op = function(args)
        {
            cocos2dx.objects.asyncImageLoader.loadImage(args.path, args.data);
        };
        cocos2dx.objects.asyncImageLoader.operationQueue.enqueue(op, opArgs);
    }
};

autoAddDeps(LibraryCocosHelper, '$cocos2dx');
mergeInto(LibraryManager.library, LibraryCocosHelper);

