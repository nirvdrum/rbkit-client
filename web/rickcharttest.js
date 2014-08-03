// Generated by CoffeeScript 1.7.1
var colorPalette, counter, generateFillerSeries, graph, isRegistered, mappedClasses, notMappedClasses, receiveObjectData, tryQtBridge;

counter = 0;

mappedClasses = [];

colorPalette = new Rickshaw.Color.Palette();

generateFillerSeries = function(count) {
  var dummyData, num, _i, _ref;
  if (count === 0) {
    return [];
  }
  dummyData = [];
  for (num = _i = 0, _ref = count - 1; 0 <= _ref ? _i <= _ref : _i >= _ref; num = 0 <= _ref ? ++_i : --_i) {
    dummyData.push({
      x: num,
      y: 0
    });
  }
  return dummyData;
};

isRegistered = function(klass) {
  return mappedClasses.indexOf(klass) > -1;
};

notMappedClasses = function(objectData) {
  var klasses;
  klasses = Object.keys(objectData);
  return klasses.filter(function(element) {
    return mappedClasses.indexOf(element) < 0;
  });
};

receiveObjectData = function(objectData) {
  var klass, unMappedClasses, _i, _len;
  unMappedClasses = notMappedClasses(objectData);
  for (_i = 0, _len = unMappedClasses.length; _i < _len; _i++) {
    klass = unMappedClasses[_i];
    graph.series.addItem({
      name: klass,
      color: colorPalette.color()
    });
    mappedClasses.push(klass);
  }
  return graph.series.addData(objectData);
};

graph = new Rickshaw.Graph({
  element: document.querySelector('#chart'),
  width: document.width - 30,
  height: document.height - 30,
  renderer: 'bar',
  series: new Rickshaw.Series.FixedDuration([
    {
      name: '',
      color: colorPalette.color()
    }
  ], void 0, {
    timeInterval: 100,
    maxDataPoints: 100,
    timeBase: new Date().getTime() / 1000
  })
});

tryQtBridge = function() {
  if (window.rbkitClient) {
    window.rbkitClient.sendDatatoJs.connect(receiveObjectData);
    return graph.render();
  }
};

setInterval(tryQtBridge, 1000);

new Rickshaw.Graph.Axis.Time({
  graph: graph
});

new Rickshaw.Graph.Axis.Y.Scaled({
  graph: graph,
  tickFormat: Rickshaw.Fixtures.Number.formatKMBT,
  scale: d3.scale.log()
});

new Rickshaw.Graph.HoverDetail({
  graph: graph,
  yFormatter: function(y) {
    return "Count: " + y;
  }
});
