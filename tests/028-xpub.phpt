--TEST--
Test send / recv with XPUB and XSUB sockets
--SKIPIF--
<?php 
    require_once(dirname(__FILE__) . '/skipif.inc'); 
    if(!defined('ZMQ::SOCKET_XPUB')) die('skip');
?>
--FILE--
<?php

include dirname(__FILE__) . '/zeromq_test_helper.inc';

$context = new ZMQContext();
$server = new ZMQSocket($context, ZMQ::SOCKET_XPUB);
$server->bind(ZEROMQ_TEST_DSN);
sleep(1);
$client = new ZMQSocket($context, ZMQ::SOCKET_SUB);
$client->setSockOpt(ZMQ::SOCKOPT_SUBSCRIBE, "Hel");
$client->connect(ZEROMQ_TEST_DSN);
sleep(1);
var_dump(substr($server->recvmsg(), 1));
$server->sendmsg("Goodbye world!");
$server->sendmsg("Hello world!");

$message = $client->recvmsg();
var_dump($message);

--EXPECT--
string(3) "Hel"
string(12) "Hello world!"
