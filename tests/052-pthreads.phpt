--TEST--
Test pthreads integration
--SKIPIF--
<?php 
	require_once(dirname(__FILE__) . '/skipif.inc'); 
	if (!extension_loaded ('pthreads')) {
		die ('skip Requires pthreads extension');
	}
	if (!in_array ('acquire', get_class_methods ('ZMQContext')))
		die ('skip');
	if (!in_array ('getsocketcount', get_class_methods ('ZMQContext')))
		die ('skip');
?>
--FILE--
<?php

$threads = 10;

class MyWorker extends Thread {
	public $sendThisBack;

	public function __construct($sendThisBack){
		$this->sendThisBack = $sendThisBack;
	}

	public function run() {
		$context = ZMQContext::acquire();
		$socket = $context->getSocket(ZMQ::SOCKET_PUSH);
		$socket->connect ("inproc://pthreads-test");
		$socket->send($this->sendThisBack);
		sleep(2);
	}
}

class MyServer extends Thread {
    public $socket;

    public function run() {
        $context = ZMQContext::acquire();
        $this->socket = $context->getSocket(ZMQ::SOCKET_PULL);
        $this->socket->bind("inproc://pthreads-test");
        $this->socket->setSockOpt(ZMQ::SOCKOPT_HWM, 1000);
        sleep(2);
    }
}

$server = new MyServer();
$server->start();
echo 'Server started' . PHP_EOL;

$requests = array();
for ($i = 0; $i < $threads; $i++) {
	$requests[$i] = new MyWorker("thr_$i");
	$requests[$i]->start();
}
echo 'Workers started' . PHP_EOL;

$context = ZMQContext::acquire();
var_dump($context->getSocketCount());

for ($i = 0; $i < $threads; $i++) {
	$requests[$i]->join();
}
$server->join();
echo 'All requests pushed' . PHP_EOL;

for ($i = 0; $i < $threads; $i++) {
	$server->socket->recv();
}

echo "OK";

--EXPECT--
Server started
Workers started
int(11)
All requests pushed
OK
