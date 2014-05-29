var s = require('net').Socket();
s.connect(8080);
s.write('dummy call');
s.end();
