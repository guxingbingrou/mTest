var isInitiator
room = prompt('Enter room name:');
const socket = io.connect();

if(room !== ''){
	console.log('Joining room ' + room);
	socket.emit('create or join', room);
}
socket.on('full', (room) => {
	console.log('Room ' + room + ' is full');
});
socket.on('empty', (room) => {
	isInitiator = true;
	console.log('Room ' + room + ' is empty');
});
socket.on('join', (room) => {
	console.log('Makeing request to join room' + room);
	console.log('You are the initiator!');
});
socket.on('log', (array) => {
	console.log.apply(console, array);
});
