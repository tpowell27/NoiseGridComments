//
//  ofApp.cpp
//  NoiseGrid
//
//  Created by Andreas Müller.
//  Commented by Travis Powell.

#include <stdio.h>
#include "ofApp.h"

// --------------------------------
void ofApp::setup()
{
    fontSmall.loadFont("Fonts/DIN.otf", 8 );
    
    //Builds the gui interface and sets the fonts and positions of each slider and button.
    ofxGuiSetFont( "Fonts/DIN.otf", 8 );
    ofxGuiSetDefaultWidth( 260 );
    
    string xmlSettingsPath = "Settings/NoiseGrid.xml";
    gui.setup( "Noise Grid", xmlSettingsPath );
    
    gui.add( frequency.set("Frequency",  1.0f,  0.01f,  2.0f) );
    gui.add( timeFrequency.set("Time Frequency",  1.0f,  0.001f,  3.0f) );
    gui.add( magnitude.set("Magnitude",  0.3f,  0.0f,  5.0f) );
    
    gui.add( gridSize.set("Grid Size",  6.0f,  0.01f,  20.0f) );
    gui.add( gridResolution.set("Grid Resolution",  20,  3,  100) );
    
    gui.loadFromFile( xmlSettingsPath );
    
    //Disables auto distance to prevent the camera from constantly moving, sets position, field of view, focal point of the camera.
    camera.setAutoDistance( false );
    camera.setGlobalPosition( ofVec3f(0,5,-10) );
    camera.lookAt( ofVec3f(0,3,0) );
    camera.setNearClip( 0.0001f );
    camera.setFarClip( 512 );
    
    //Initiates the gui so that it can be drawn in the key pressed function.
    drawGui = true;
}


// --------------------------------
void ofApp::update()
{
    //Sets the variables used to animate the particles.
    float time = ofGetElapsedTimef();
    float noiseTime = time * timeFrequency;
    
    //Sets the variables used to adjust the size of the particle grid.
    ofVec3f extentMin( gridSize * -0.5, 0,		  gridSize * -0.5 );
    ofVec3f extentMax( gridSize *  0.5, gridSize, gridSize *  0.5 );
    
    //Clears the trails of the particles.
    mesh.clear();
    mesh.setMode( OF_PRIMITIVE_LINES );
    
    //Positions each individual particle on the grid in an array.
    for( int y = 0; y < gridResolution; y++ )
    {
        for( int x = 0; x < gridResolution; x++ )
        {
            for( int z = 0; z < gridResolution; z++ )
            {
                ofVec3f pos;
                
                pos.x = ofMap( x,  0, gridResolution-1, extentMin.x, extentMax.x );
                pos.y = ofMap( y,  0, gridResolution-1, extentMin.y, extentMax.y );
                pos.z = ofMap( z,  0, gridResolution-1, extentMin.z, extentMax.z );
                
                ofVec3f noisePos = pos * frequency;
                
                ofVec3f vel;
                vel.x = ofSignedNoise( noisePos.x, noisePos.y, noisePos.z, noiseTime );
                vel.y = ofSignedNoise( noiseTime,  noisePos.z, noisePos.y, noisePos.x );
                vel.z = ofSignedNoise( noisePos.z, noiseTime,  noisePos.y, noisePos.x );
                
                ofVec3f normVel = vel.getNormalized();
                ofVec3f velAsCol = (normVel + ofVec3f(1)) * 0.5;
                
                // Color by direction
                ofFloatColor tmpCol( velAsCol.x, velAsCol.y, velAsCol.z );
                
                mesh.addVertex( pos );
                mesh.addColor( tmpCol );
                
                mesh.addVertex( pos + (vel * magnitude));
                mesh.addColor( ofFloatColor(tmpCol, 0.0) );
            }
        }
    }
    
    
}

// --------------------------------
void ofApp::draw()
{
    ofBackgroundGradient( ofColor(40,40,40), ofColor(0,0,0), OF_GRADIENT_CIRCULAR);
    
    //Draws particles according to the z position instead of draw order.
    ofEnableDepthTest();
    
    camera.begin();
    
				// draw a grid on the floor
				ofSetColor( ofColor(60) );
    ofPushMatrix();
    ofRotate(90, 0, 0, -1);
    ofDrawGridPlane( 10, 10, false );
				ofPopMatrix();
    
				ofSetColor( ofColor::white );
				//ofDrawBox( 1.0f );
				mesh.draw();
    
    camera.end();
    
    
    ofDisableDepthTest();
    if( drawGui ) { gui.draw(); }
    
    ofSetColor( ofColor::black );
    fontSmall.drawString( ofToString(ofGetFrameRate(), 1), ofGetWidth() - 25, ofGetHeight() - 5 );
}

// --------------------------------
void ofApp::keyPressed( int _key )
{
    //Shows the gui interface when TAB is pressed.
    if( _key == ' ' )			  { }
    else if( _key == OF_KEY_TAB ) { drawGui = !drawGui; }
    else if( _key == 'f' )		  { ofToggleFullscreen(); }
    
}

