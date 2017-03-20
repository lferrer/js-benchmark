// route logs to browser window
console.log = function(msg) {
	document.body.innerHTML += msg + "<br>";
}

// The urls of the images we're going to test
var urls = [[ 
'data/friends/486b7a_a5844ee677f64925a08a3dd78c808cc8-mv2.jpg',  'data/friends/large-group-of-travelers.jpg',
'data/friends/532969250.jpg',                                    'data/friends/tv-shows-11.jpg',
'data/friends/kidsfriends.jpg' ],[
'data/caltech/image_0001.jpg',  'data/caltech/image_0004.jpg',  'data/caltech/image_0007.jpg',  'data/caltech/image_0010.jpg',  'data/caltech/image_0013.jpg',
'data/caltech/image_0002.jpg',  'data/caltech/image_0005.jpg',  'data/caltech/image_0008.jpg',  'data/caltech/image_0011.jpg',  'data/caltech/image_0014.jpg',
'data/caltech/image_0003.jpg',  'data/caltech/image_0006.jpg',  'data/caltech/image_0009.jpg',  'data/caltech/image_0012.jpg',  'data/caltech/image_0015.jpg' ],[
'data/ffi/aerosmith-double.gif',  'data/ffi/eugene.gif',           'data/ffi/life7422.gif',
'data/ffi/baseball.gif',          'data/ffi/hendrix1-bigger.gif',  'data/ffi/mom-baby.gif',
'data/ffi/blues-double.gif',      'data/ffi/hendrix2.gif',         'data/ffi/music-groups-double.gif',
'data/ffi/boat.gif',              'data/ffi/henry.gif',            'data/ffi/next.gif',
'data/ffi/book.gif',              'data/ffi/judybats.gif',         'data/ffi/original1.gif',
'data/ffi/cnn1085.gif',           'data/ffi/kaari1.gif',           'data/ffi/original2.gif',
'data/ffi/cnn1160.gif',           'data/ffi/kaari2.gif',           'data/ffi/pace-university-double.gif',
'data/ffi/cnn1260.gif',           'data/ffi/kaari-stef.gif',       'data/ffi/people.gif',
'data/ffi/cnn1630.gif',           'data/ffi/knex0.gif',            'data/ffi/plays.gif',
'data/ffi/cnn1714.gif',           'data/ffi/knex20.gif',           'data/ffi/puneet.gif',
'data/ffi/cnn2020.gif',           'data/ffi/knex37.gif',           'data/ffi/shumeet.gif',
'data/ffi/cnn2221.gif',           'data/ffi/knex42.gif',           'data/ffi/tammy.gif',
'data/ffi/cnn2600.gif',           'data/ffi/lacrosse.gif',         'data/ffi/voyager2.gif',
'data/ffi/ds9.gif',               'data/ffi/life2100.gif',         'data/ffi/voyager.gif'	

]];



function classifyJS() {
    return new Promise(function (resolve, reject) {
        for(var i = 0; i < images.length; i++){
			ccv.detect_faces(images[i]);
		}
        resolve();
    });
}

function classifyClosure() {
    return new Promise(function (resolve, reject) {
        for(var i = 0; i < images.length; i++){
			closure_ccv.detect_faces(images[i]);
		}
        resolve();
    });
}

// Cache the classifier object 
let scdCascade = null;
function classifyEmscripten() {
	return new Promise(function (resolve, reject) {
        for(var i = 0; i < images.length; i++){
			const image = new CCV.ccv_dense_matrix_t();
			CCV.ccv_read(images[i], image, CCV.CCV_IO_RGB_COLOR);
			scdCascade = scdCascade || [CCV.ccv_scd_classifier_cascade_read(CCV.CCV_SCD_FACE_FILE)];
			const params = {
				interval: 5,
				step_through: 4,
				size: {
					width: 48,
					height: 48
				},
				min_neighbors: 1
			};
			CCV.ccv_scd_detect_objects(image, scdCascade, 1, params);
		}
        resolve();
    });
}

// Different classifiers
var classifiers = {
	'js-CCV': classifyJS,
	'closure-CCV': classifyClosure,
    'emscripten-CCV': classifyEmscripten
};

// We store the image data in this array
var images = [];

// called when buffer is filled
function executeBenchmarks()
{
	return new Promise(function (resolve, reject) {

		// create a new benchmark suite
		var suite = new Benchmark.Suite;

		// patched benchmark reporting
		function report(type) {
			return " Elapsed time: " + this.times.elapsed;
		}

		// setup test for each solver
		for (var type in classifiers) {
            (function(type) {
				suite.add(type, function(deferred) {
					classifiers[type]()
					.then(function (result) {
						deferred.resolve();						
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
			resolve();
		})
		.run({ 'async': true });
	})
}

// auto advancing test function
function testAssets(set)
{
	set = set || 0;
	promises = [];
	images = [];
	console.log('TESTING: ' + urls[set].length +  ' images.');
    for (var i = 0; i < urls[set].length; i++) {
		var image = new Image();
		image.src = urls[set][i];
		images[images.length] = image;
        promises.push(image.onload);
    }
	// wait until all urls are loaded
	Promise.all(promises)
	// invoke test runner
	.then(executeBenchmarks)
	// schedule more sets
	.then(function()
	{
		if (++ set < urls.length) {
			testAssets(set); // next
		}
	})	
}

