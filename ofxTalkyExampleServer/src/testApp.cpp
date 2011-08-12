#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

	talkyServer.setup(5000);

	ofSetWindowShape(400, 700);
	ofBackground(200, 50, 50);
	ofSetVerticalSync(true);
}

//--------------------------------------------------------------
void testApp::update(){
	status = "fps: " + ofToString(ofGetFrameRate(), 0) + "\n";
	status += "talkyServer\n";
	status += "bound to port: ";
	status += (talkyServer.getIsServerBound() ? "true" : "false");
	status += "\nClients connected: " + ofToString(talkyServer.getNumClients());
	status += "\n";
	status += "receiveQueue count: " + ofToString(talkyServer.receiveQueue.size(), 0) + "\n";
	status += "sendQueue count: " + ofToString(talkyServer.getSendQueue().size(), 0) + "\n";
	
	
	TalkyMessage msg;
	ofPoint point;
	while (talkyServer >> msg)
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
	
	ofEnableSmoothing();
	
	ofNoFill();
	ofSetLineWidth(3);
	ofBeginShape();
	
	for (int i=0; i<line.size(); i++)
		ofCurveVertex(line[i].x, line[i].y);
	
	ofEndShape(false);
	
	ofDrawBitmapString(status, 10, 30);
	
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
	msg.setCompany("KC");
	msg.setProtocol("mo");
	msg.setPayload((char *) &point, sizeof(ofPoint));
	msg.Version = 1;
	msg.ContentsType = 1;
	
	talkyServer.send(msg);
	
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

