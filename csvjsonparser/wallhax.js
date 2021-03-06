var ticketLocations = "";

var columnNames = {};
columnNames[0] = "todo";
columnNames[1] = "inprogress";
columnNames[2] = "done";


var columnWidth = 682;

process.stdin.resume();
process.stdin.setEncoding('utf8');

process.stdin.on('data', function (lines) {
	lines.split(/\n/).forEach(function(line){
		if ((/.+,.+,.+/).test(line)) {
			splitLine = line.split(/,/);
			var ticketId = splitLine[2];
	 		var xPos = splitLine[0];
	 		var columnInd = Math.floor(xPos/columnWidth);
	 		columnInd = Math.min(columnInd, 2);
	 		columnInd = Math.max(columnInd, 0);
	 		ticketLocations += columnNames[columnInd] + " " + ticketId+"\n";
		}
	})
	console.log(ticketLocations);
});

