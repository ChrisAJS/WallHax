var columnNameToPosition = {};
var ticketIdToPosition = {};
var ticketLocations = "";
process.stdin.resume();
process.stdin.setEncoding('utf8');

process.stdin.on('data', function (lines) {
	lines.split(/\n/).forEach(function(line){
		if ((/.+,.+,.+/).test(line)) {
			var splitLine = line.split(/,/);
			if ((/Column:.+/).test(splitLine[2])) {
				columnNameToPosition[splitLine[2].toLowerCase().replace(/column:|\s/g, "")] = parseFloat(splitLine[0]);
			}
			else {
				ticketIdToPosition[splitLine[2]] = parseFloat(splitLine[0]);
			}
		}
	})

	for(ticketId in ticketIdToPosition) {
		var columnEstimate = null;
		for (columnName in columnNameToPosition) {
			if (columnNameToPosition[columnName] < ticketIdToPosition[ticketId] &&
				(columnEstimate == null || columnNameToPosition[columnName] > columnNameToPosition[columnEstimate])) {
				columnEstimate = columnName;
			}
		}
		ticketLocations += columnEstimate + " " + ticketId+"\n";
	}
	console.log(ticketLocations);
});

