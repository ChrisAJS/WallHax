var jsonObject = 
	[{
	    "columnName": "todo",
	    "tickets": []
	}, {
	    "columnName": "indev",
	    "tickets": []
	}, {
	    "columnName": "done",
	    "tickets": []
	}];

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
	 		jsonObject[columnInd].tickets.push(ticketId);
		}
	})
	console.log(jsonObject);
});

