/*
 * Simple script for running emcc on quirc
 * @author zz85 github.com/zz85
 */


var
	exec = require('child_process').exec,
	path = require('path'),
	fs = require('fs'),
	child;

var HAVE_NFT = 0;

var EMSCRIPTEN_PATH = process.env.EMSCRIPTEN;

if (!EMSCRIPTEN_PATH) {
	console.log("\nWarning: EMSCRIPTEN environment variable not found.")
	console.log("If you get a \"command not found\" error,\ndo `source <path to emsdk>/emsdk_env.sh` and try again.");
	process.exit(1)
}

var EMCC = EMSCRIPTEN_PATH ? path.resolve(EMSCRIPTEN_PATH, 'emcc') : 'emcc';
var OPTIMIZE_FLAGS = ' -Oz '; // -Oz for smallest size
var MEM = 32 * 1024 * 1024; // 64MB

var SOURCE_PATH = path.resolve(__dirname, 'lib') + '/';
var OUTPUT_PATH = path.resolve(__dirname, '.') + '/';
var BUILD_FILE = 'quirc.js';

var MAIN_SOURCES = [
	'*.c'
].map(function(x) {
	return SOURCE_PATH + x
})

MAIN_SOURCES.push('quircjs.cpp');

var DEFINES = ' ';

var FLAGS = '' + OPTIMIZE_FLAGS;
// FLAGS += ' -Wno-warn-absolute-paths ';
FLAGS += ' -s TOTAL_MEMORY=' + MEM + ' ';
FLAGS += ' -s NO_BROWSER=1 '; // for 20k less
FLAGS += ' --memory-init-file 0 '; // for memless file


var funcs = 'xsetup,xprocess'
var FUNCS = funcs.split(',').map(function(func) { return "'_" + func + "'" }).join(',')
FLAGS += ' -s EXPORTED_FUNCTIONS="[' + FUNCS + ']" '


// FLAGS += ' --bind ';

/* DEBUG FLAGS */

var DEBUG_FLAGS = ' -g ';
// DEBUG_FLAGS += ' -s ASSERTIONS=2 '
// DEBUG_FLAGS += ' -s ASSERTIONS=1 '
// DEBUG_FLAGS += ' --profiling-funcs '
// DEBUG_FLAGS += ' -s EMTERPRETIFY_ADVISE=1 '
// DEBUG_FLAGS += ' -s ALLOW_MEMORY_GROWTH=1';
// DEBUG_FLAGS += '  -s DEMANGLE_SUPPORT=1 ';
// DEBUG_FLAGS += ' -s DISABLE_EXCEPTION_CATCHING=0 ';

// FLAGS += DEBUG_FLAGS


var INCLUDES = [
	// path.resolve(__dirname, '../../include'),
	SOURCE_PATH,
].map(function(s) { return '-I' + s }).join(' ');

function format(str) {
	for (var f = 1; f < arguments.length; f++) {
		str = str.replace(/{\w*}/, arguments[f]);
	}
	return str;
}

var compile_all = format(EMCC + ' ' + INCLUDES + ' '
	+ MAIN_SOURCES.join(' ')
	+ FLAGS + ' ' + DEFINES + ' -o {OUTPUT_PATH}{BUILD_FILE} ',
		OUTPUT_PATH, BUILD_FILE);

/*
 * Run commands
 */

function onExec(error, stdout, stderr) {
	if (stdout) console.log('stdout: ' + stdout);
	if (stderr) console.log('stderr: ' + stderr);
	if (error !== null) {
		console.log('exec error: ' + error);
	} else {
		runJob();
	}
}

function runJob() {
	if (!jobs.length) {
		console.log('Jobs completed');
		return;
	}
	var cmd = jobs.shift();

	if (typeof cmd === 'function') {
		cmd();
		runJob();
		return;
	}

	console.log('\nRunning command: ' + cmd + '\n');
	exec(cmd, onExec);
}

var jobs = [];

function addJob(job) {
	jobs.push(job);
}

addJob(compile_all);

runJob();