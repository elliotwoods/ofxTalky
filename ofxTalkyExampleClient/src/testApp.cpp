#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

	talkyClient.setup("127.0.0.1");

	ofSetWindowShape(400, 700);
	ofBackground(50, 200, 50);
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	
	msgHeader = TalkyMessageHeader("KC", "mo", 1, 1);
}

//--------------------------------------------------------------
void testApp::update(){
	
	status = "fps: " + ofToString(ofGetFrameRate(), 0) + "\n";
	status += "talkyClient\n";
	status += "connected: ";
	status += (talkyClient.getIsConnected() ? "true" : "false");
	status += "\n";
	status += "receiveQueue count: " + ofToString(talkyClient.getReceiveQueue().size(), 0) + "\n";
	status += "sendQueue count: " + ofToString(talkyClient.getSendQueue().size(), 0) + "\n";
	
	
	TalkyMessage msg;
	ofPoint point;
	while (talkyClient >> msg)
	{
		status += msg.toString();
		msg >> point;
		
		line.push_back(point);
	}
	
	while (line.size() > 200)
		line.erase(line.begin(), line.begin() + 1);
}

//--------------------------------------------------------------
void testApp::draw(){
	
	ofDrawBitmapString(status, 10, 30);
	
	if (line.size() == 0)
		return;
	
	ofEnableSmoothing();
	
	ofNoFill();
	ofSetLineWidth(3);
	ofBeginShape();
	
	for (int i=0; i<line.size(); i++)
		ofCurveVertex(line[i].x, line[i].y);
	
	ofEndShape(false);
	
	ofCircle(line.back().x, line.back().y, 10);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	
	ofPoint point = ofPoint(x,y);
	
	TalkyMessage msg;
	msg.setHeader(msgHeader);
	msg << point;
	
	talkyClient.send(msg);
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

