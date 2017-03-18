// route logs to browser window
console.log = function(msg) {
	document.body.innerHTML += msg + "<br>";
}

// How many boxes we will simulate
var amounts = [ 10, 100, 200 ];

function runTest(data) {
    return new Promise(function (resolve, reject) {
        var dt = 1;
        if (typeof(data.Step) === "undefined") {
            data.sd(dt, 2, 2);
        }
        else {
            data.Step(dt, 2, 2);
        }        
        resolve();
    });
}

// Different generators
var generators = {
	'js-Box2D': generateBoxesJS,
	'closure-Box2D': generateBoxesClosure,
    'emscripten-Box2D': generateBoxesEmscripten
};

// Placeholders for the world/bodies
var benchData = {
    'js-Box2D': null,
	'closure-Box2D': null,
    'emscripten-Box2D': null
};

// called when buffer is filled
function executeBenchmarks()
{
	size_in = {}, size_out = {};
	return new Promise(function (resolve, reject) {

		// create a new benchmark suite
		var suite = new Benchmark.Suite;

		// patched benchmark reporting
		function report(type) {
			return " Elapsed time: " + this.times.elapsed;
		}

		// setup test for each solver
		for (var id in libraries) {
            var type = libraries[id];
			(function(type) {
				suite.add(type, function(deferred) {
					runTest(benchData[type])
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

// Generate boxes
function generateBoxesJS(NUM)
{
	return new Promise(function (resolve, reject) {        
        benchData['js-Box2D'] = JSBox2D.BoxBenchmark(NUM);
		resolve();
	})
}

function generateBoxesClosure(NUM)
{
	return new Promise(function (resolve, reject) {        
        benchData['closure-Box2D'] = V.Sm(NUM);
		resolve();
	})
}

// Emscripten decided to collapse the namespaces -_-
function generateBoxesEmscripten(NUM)
{
	return new Promise(function (resolve, reject) {
        var NUMRANGE = [];
        while (NUMRANGE.length < NUM) NUMRANGE.push(NUMRANGE.length+1);
        var bodies = [null]; // Indexes start from 1

        // Box2D-interfacing code
        var gravity = new EmscriptenBox2D.b2Vec2(0.0, -10.0);
        var world = new EmscriptenBox2D.b2World(gravity);
        var bd_ground = new EmscriptenBox2D.b2BodyDef();
        var ground = world.CreateBody(bd_ground);

        var shape0 = new EmscriptenBox2D.b2EdgeShape();
        shape0.Set(new EmscriptenBox2D.b2Vec2(-40.0, -6.0), new EmscriptenBox2D.b2Vec2(40.0, -6.0));
        ground.CreateFixture(shape0, 0.0);

        var size = 1.0;
        var shape = new EmscriptenBox2D.b2PolygonShape();
        shape.SetAsBox(size, size);

        var ZERO = new EmscriptenBox2D.b2Vec2(0.0, 0.0);
        var temp = new EmscriptenBox2D.b2Vec2(0.0, 0.0);

        NUMRANGE.forEach(function(i) {
            var bd = new EmscriptenBox2D.b2BodyDef();
            bd.set_type(EmscriptenBox2D.b2_dynamicBody);
            bd.set_position(ZERO);
            var body = world.CreateBody(bd);
            body.CreateFixture(shape, 5.0);
            bodies.push(body);
        });

        benchData['emscripten-Box2D'] = world;
		resolve();
	})
}

// auto advancing test function
function testAssets(set)
{
	set = set || 0;
	var promises = [];
	console.log('TESTING: ' + amounts[set] +  ' boxes.');
    for(var i = 0; i < libraries.length; i++) {
        var generator = generators[libraries[i]];
	    promises.push(generator(amounts[set]));
    }

	// wait until all urls are loaded
	Promise.all(promises)
	// invoke test runner
	.then(executeBenchmarks)
	// schedule more sets
	.then(function()
	{
		if (++ set < amounts.length) {
			testAssets(set); // next
		}
	})
}

