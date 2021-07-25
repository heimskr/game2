#!/usr/bin/env node
const fs = require("fs");
const {spawn} = require("child_process");

if (!fs.existsSync("game.exe")) {
	console.error("Game executable not found.");
	process.exit(1);
}

if (fs.existsSync("game2")) {
	console.error("game2 file/directory already exists.");
	process.exit(1);
}

fs.mkdirSync("game2");

const ldd = spawn("ldd", ["ldd", "game.exe"]);
let output = "";

ldd.stdout.on("data", data => { output += data.toString("utf8"); });

ldd.on("close", code => {
	const split = output.split(/[\r\n]+/);
	for (const line of split) {
		if (!line.match("=>"))
			continue;
		let filename = line.replace(/^.+=> ([^(]+) \(.+/, "$1");
		if (!filename.match(/^\/mingw64\//))
			continue;
		const base = filename.replace(/^.+\/([^\/]+)$/, "$1");
		filename = "E:/msys64" + filename;
		fs.copyFileSync(filename, "game2/" + base); 
	}

	fs.copyFileSync("game.exe", "game2/game.exe");
	// fs.rmSync("game2", {recursive: true});
});
