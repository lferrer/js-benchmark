// urls to load for tests
var urls = [
	[
		['js-lzma', 'data/stars.names.json.lzma'],
		['closure-lzma', 'data/stars.names.json.lzma'],
		['emscripten-lzma', 'data/stars.names.json.ems']
	], [
		['js-lzma', 'data/stars.col.db.lzma'],
		['closure-lzma', 'data/stars.col.db.lzma'],
		['emscripten-lzma', 'data/stars.col.db.ems']
	], [
		['js-lzma', 'data/stars.pos.db.lzma'],
		['closure-lzma', 'data/stars.pos.db.lzma'],
		['emscripten-lzma', 'data/stars.pos.db.ems']
	]
];

// name mappings
var names = [
	'names',
	'col',
	'pos'
];

// route logs to browser window
console.log = function(msg) {
	document.body.innerHTML += msg + "<br>";
}

function decodeJsLZMA(buffer)
{
	return new Promise(function (resolve, reject) {
		var input = new JSLZMA.iStream(buffer);
		var output = new JSLZMA.oStream();
		JSLZMA.decompressFile(input, output);
		resolve(output.toUint8Array());
	});
}

function decodeClosureLZMA(buffer)
{
	return new Promise(function (resolve, reject) {
		//These are the new names that closure gave to our functions
		var input = new CLOSURE_LZMA.ma(buffer);
		var output = new CLOSURE_LZMA.B();
		CLOSURE_LZMA.za(input, output);
		resolve(output.pa());
	});
}

function decodeEmscriptenLZMA(buffer) {
    return new Promise(function (resolve, reject) {
        var input = new Uint8Array(buffer);
        var output = EMSCRIPTEN_LZMA.decompress(input);
        resolve(output);
    });
}

// the decoder to test
var decoder = {
	'js-lzma': decodeJsLZMA,
	'closure-lzma': decodeClosureLZMA,
    'emscripten-lzma': decodeEmscriptenLZMA
};

// statistics
var size_in = {},
    size_out = {};

// fetched async
var buffers = {};

// conversion factor
var B2MB = 1024 * 1024;	

// helper function
function formatNr(nr)
{
	return nr.toFixed(nr < 100 ? 2 : 0);
}

// called when buffer is filled
function loadedBuffers()
{
	size_in = {}, size_out = {};
	return new Promise(function (resolve, reject) {

		// create a new benchmark suite
		var suite = new Benchmark.Suite;

		// patched benchmark reporting
		function report(type) {
			var tp = size_out[type] / B2MB;
			tp *= this.stats.sample.length;
			tp /= this.times.elapsed;
			var ratio = 100 * size_in[type] / size_out[type];
			console.warn(size_out[type], size_in[type], ratio)
			tp = formatNr(tp), ratio = formatNr(ratio);
			return " (" + tp + " MB/s - " + ratio + "%)";
		}

		// setup test for each decoder
		for (var type in decoder) {
			(function(type) {
				suite.add(type, function(deferred) {
					size_in[type] = buffers[type].byteLength;
					decoder[type](buffers[type])
					.then(function (result) {
						deferred.resolve();
						if (size_out[type]) {
							if (size_out[type] != result.length) {
								throw Error("Uncompressed sizes vary!?");
							}
						}
						size_out[type] = result.length;
					})
				}, { defer: true, report: function () {
					return report.call(this, type);
				} })
			})(type);
		}

		// setup suite
		suite
		.on('cycle', function(event) {
			console.log(String(event.target));
		})
		.on('complete', function() {
			// console.log('Fastest is ' + this.filter('fastest').map('name'));
			resolve();
		})
		// run async
		.run({ 'async': true });
	})
}

// load url for test suite
function loadUrl(type, url)
{
	return new Promise(function (resolve, reject) {
		var oReq = new XMLHttpRequest();
		oReq.open("GET", url, true);
		oReq.responseType = "arraybuffer";
		oReq.onload = function(oEvent)
		{
			// if response contains valid content-encoding
			// header, the UA will already unwrap content!
			buffers[type] = oReq.response;
			// if you want to access the bytes:
			// var byteArray = new Uint8Array(arrayBuffer);
			// If you want to use the image in your DOM:
			// var blob = new Blob(arrayBuffer, {type: "image/png"});
			// var url = URL.createObjectURL(blob);
			// someImageElement.src = url;
			resolve();
		};
		oReq.send();
	})
}

// auto advancing test function
function testAssets(set)
{
	set = set || 0;
	var promises = [];
	console.log('TESTING: ' + names[set]);
	for(var i = 0; i < urls[set].length; i++) {
		var type = urls[set][i][0],
			url = urls[set][i][1];
		promises.push(loadUrl(type, url));
	}
	// wait until all urls are loaded
	Promise.all(promises)
	// invoke test runner
	.then(loadedBuffers)
	// schedule more sets
	.then(function()
	{
		if (++ set < urls.length) {
			testAssets(set); // next
		}
	})
}

