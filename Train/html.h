#ifndef HTML_H
#define HTML_H

/*
  NOTE: This file is generated, do not edit directly
  
  Edit files in html/ and run buildheader.sh to update
*/
const char index_html[] = PROGMEM {"<!DOCTYPE html>\n\
<html>\n\
  <head>\n\
    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n\
	<title>Control</title>\n\
	<style>\n\
	  body {\n\
		  font-family: Sans-Serif;\n\
		  Color: #000088;\n\
		  width: 100%;\n\
	  }\n\
	</style>\n\
	<script src=\"app.js\" type=\"application/javascript\"></script>\n\
  </head>\n\
  <body>\n\
	<h1 id=\"title\"></h1>\n\
	<div>\n\
	  <h2>Control</h2>\n\
	  <form id=\"control\">\n\
		Direction: \n\
		<input type=\"radio\" name=\"dir\" value=\"F\" checked> Forward \n\
		<input type=\"radio\" name=\"dir\" value=\"R\"> Reverse<br>\n\
		Speed: <input type=\"range\" name=\"speed\" min=\"0\" max=\"100\"  oninput=\"this.nextElementSibling.value = this.value\" value=\"50\"><output>50</output><br>\n\
		Target tag:\n\
		<select name=\"rfid\">\n\
		  <option value=\"\">None</option>\n\
		  <option value=\"next\">Next</option>\n\
		</select><br>\n\
		<input type=\"submit\" value=\"Go\">\n\
		<input type=\"submit\" value=\"Stop\">		\n\
	  </form>\n\
	</div>\n\
	<div>\n\
	  <h2>Config</h2>\n\
	  <form id=\"config\">\n\
		Name: <input type=\"text\" name=\"name\" value=\"\"><br>\n\
		Battery empty: <input type=\"range\" name=\"batteryEmpty\" oninput=\"this.nextElementSibling.value = this.value\" min=\"0\" max=\"1023\"><output></output><br>\n\
		Battery full: <input type=\"range\" name=\"batteryFull\" oninput=\"this.nextElementSibling.value = this.value\" min=\"0\" max=\"1023\"><output></output><br>\n\
		Motor min: <input type=\"range\" name=\"motorMin\" oninput=\"this.nextElementSibling.value = this.value\" min=\"0\" max=\"255\"><output></output><br>\n\
		Motor max: <input type=\"range\" name=\"motorMax\" oninput=\"this.nextElementSibling.value = this.value\" min=\"0\" max=\"255\"><output></output><br>\n\
		Active brake: \n\
		<input type=\"radio\" name=\"activeBrake\" value=\"true\" checked> On \n\
		<input type=\"radio\" name=\"activeBrake\" value=\"false\"> Off<br>\n\
		Brake time (ms): <input type=\"range\" name=\"brakeTimeMS\" oninput=\"this.nextElementSibling.value = this.value\" min=\"0\" max=\"2000\"><output></output><br>\n\
		Beep on tag read: \n\
		<input type=\"radio\" name=\"beep\" value=\"true\" checked> On \n\
		<input type=\"radio\" name=\"beep\" value=\"false\"> Off<br>\n\
		Beep time (ms): <input type=\"range\" name=\"beepMS\" oninput=\"this.nextElementSibling.value = this.value\" min=\"10\" max=\"1000\"><output></output><br>\n\
		Display raw control line values:  <br>\n\
		<input type=\"radio\" name=\"displayRaw\" value=\"true\"> On \n\
		<input type=\"radio\" name=\"displayRaw\" value=\"false\" checked> Off<br>\n\
		Dynamically adjust motor driver to maintain output voltage to the motor as input battery voltage drops: <br>\n\
		<input type=\"radio\" name=\"dynamicPower\" value=\"true\"> On \n\
		<input type=\"radio\" name=\"dynamicPower\" value=\"false\" checked> Off<br>\n\
		<table id=\"tags\">\n\
		  <tr>\n\
			<th>Name</th>\n\
			<th>UID</th>\n\
		  </tr>\n\
		  <tr>\n\
			<td><input type=\"text\" name=\"tagName\" id=\"tagName0\"></td>\n\
			<td><input type=\"text\" name=\"tag\" id=\"tag0\"></td>\n\
		  </tr>\n\
		  <tr>\n\
			<td><input type=\"text\" name=\"tagName\" id=\"tagName1\"></td>\n\
			<td><input type=\"text\" name=\"tag\" id=\"tag1\"></td>\n\
		  </tr>\n\
		  <tr>\n\
			<td><input type=\"text\" name=\"tagName\" id=\"tagName2\"></td>\n\
			<td><input type=\"text\" name=\"tag\" id=\"tag2\"></td>\n\
		  </tr>\n\
		  <tr>\n\
			<td><input type=\"text\" name=\"tagName\" id=\"tagName3\"></td>\n\
			<td><input type=\"text\" name=\"tag\" id=\"tag3\"></td>\n\
		  </tr>\n\
		  <tr>\n\
			<td><input type=\"text\" name=\"tagName\" id=\"tagName4\"></td>\n\
			<td><input type=\"text\" name=\"tag\" id=\"tag4\"></td>\n\
		  </tr>\n\
		  <tr>\n\
			<td><input type=\"text\" name=\"tagName\" id=\"tagName5\"></td>\n\
			<td><input type=\"text\" name=\"tag\" id=\"tag5\"></td>\n\
		  </tr>\n\
		  <tr>\n\
			<td><input type=\"text\" name=\"tagName\" id=\"tagName6\"></td>\n\
			<td><input type=\"text\" name=\"tag\" id=\"tag6\"></td>\n\
		  </tr>\n\
		  <tr>\n\
			<td><input type=\"text\" name=\"tagName\" id=\"tagName7\"></td>\n\
			<td><input type=\"text\" name=\"tag\" id=\"tag7\"></td>\n\
		  </tr>\n\
		  <tr>\n\
			<td><input type=\"text\" name=\"tagName\" id=\"tagName8\"></td>\n\
			<td><input type=\"text\" name=\"tag\" id=\"tag8\"></td>\n\
		  </tr>\n\
		  <tr>\n\
			<td><input type=\"text\" name=\"tagName\" id=\"tagName9\"></td>\n\
			<td><input type=\"text\" name=\"tag\" id=\"tag9\"></td>\n\
		  </tr>\n\
		</table>\n\
		<input type=\"submit\" value=\"Submit\">\n\
	  </form>\n\
	</div>\n\
	<div>\n\
	  <pre id=\"response\"></pre>\n\
	</div>\n\
  </body>\n\
</html>\n\
"};
const char style_css[] = PROGMEM {""};
const char app_js[] = PROGMEM {"function getData(url,callback) {\n\
  var xhr = new XMLHttpRequest();\n\
  xhr.addEventListener( \"load\", function(event) {\n\
    document.getElementById(\"response\").innerHTML = JSON.stringify(JSON.parse(event.target.responseText),null,2);\n\
    if(callback) {\n\
      callback(JSON.parse(event.target.responseText));\n\
    }\n\
    buttonDisable(false);\n\
  });\n\
  xhr.addEventListener( \"error\", function( event ) {\n\
    alert('Error occurred loading ' + url);\n\
    buttonDisable(false);\n\
  });\n\
  buttonDisable(true);\n\
  xhr.open(\"GET\", url, true);\n\
  xhr.send();\n\
}\n\
\n\
function postData(url,formData,callback) {\n\
  var xhr = new XMLHttpRequest();\n\
  \n\
  xhr.addEventListener( \"load\", function(event) {\n\
    document.getElementById(\"response\").innerHTML = JSON.stringify(JSON.parse(event.target.responseText),null,2);\n\
    if(callback) {\n\
      callback(JSON.parse(event.target.responseText));\n\
    }\n\
    buttonDisable(false);\n\
  });\n\
  xhr.addEventListener( \"error\", function( event ) {\n\
    alert('Error occurred loading ' + url );\n\
    buttonDisable(false);\n\
  });\n\
  \n\
  buttonDisable(true);\n\
  xhr.open(\"POST\",url);\n\
  xhr.send(formData);\n\
}\n\
\n\
// register senders\n\
window.addEventListener(\"load\",function() {\n\
  // pull config\n\
  getData(\"/api/config\",updateConfigForm);\n\
  \n\
  // setup control form handler\n\
  var controlForm = document.getElementById(\"control\");\n\
  controlForm.addEventListener(\"submit\", function(event) {\n\
    // if rfid is blank, remove it\n\
    var formData = new FormData(controlForm);\n\
    console.log(\"rfid=\" + controlForm.elements[\"rfid\"].value);\n\
    if(formData.get(\"rfid\")==\"\") {\n\
      formData.delete(\"rfid\");\n\
    }\n\
    if(event.submitter.value==\"Stop\") {\n\
      console.log(\"Stop\");\n\
      formData.delete(\"speed\");\n\
      formData.append(\"speed\",0);\n\
      formData.delete(\"dir\");\n\
      formData.append(\"dir\",\"F\");\n\
    }\n\
    postData(\"/api/control\",formData);\n\
    event.preventDefault();\n\
  } );\n\
  \n\
  // setup config form handler\n\
  var configForm = document.getElementById(\"config\");\n\
  configForm.addEventListener(\"submit\", function(event) {\n\
    postData(\"/api/config\",new FormData(configForm),updateConfigForm);\n\
    event.preventDefault();\n\
  } );\n\
  \n\
});\n\
\n\
function updateConfigForm(config) {\n\
  document.title = config.name;\n\
  document.getElementById(\"title\").innerHTML = document.title;\n\
  \n\
  var configForm = document.getElementById(\"config\");\n\
  configForm.elements.name.value = config.name;\n\
  configForm.elements.batteryEmpty.value = config.batteryEmpty;\n\
  configForm.elements.batteryEmpty.nextElementSibling.innerText = config.batteryEmpty;\n\
  configForm.elements.batteryFull.value = config.batteryFull;\n\
  configForm.elements.batteryFull.nextElementSibling.innerText = config.batteryFull;\n\
  configForm.elements.motorMin.value = config.motorMin;\n\
  configForm.elements.motorMin.nextElementSibling.innerText = config.motorMin;\n\
  configForm.elements.motorMax.value = config.motorMax;\n\
  configForm.elements.motorMax.nextElementSibling.innerText = config.motorMax;\n\
  configForm.elements.activeBrake.value = config.activeBrake;\n\
  configForm.elements.brakeTimeMS.value = config.brakeTimeMS;\n\
  configForm.elements.brakeTimeMS.nextElementSibling.innerText = config.brakeTimeMS;\n\
  configForm.elements.displayRaw.value = config.displayRaw;\n\
  configForm.elements.dynamicPower.value = config.dynamicPower;\n\
  configForm.elements.beep.value = config.beep;\n\
  configForm.elements.beepMS.value = config.beepMS;\n\
  configForm.elements.beepMS.nextElementSibling.innerText = config.beepMS;\n\
  \n\
  // populate tag fields\n\
  for(var i=0; i<config.tags.length; i++) {\n\
    document.getElementById(\"tagName\" + i).value = config.tags[i].tagName;\n\
    document.getElementById(\"tag\" + i).value = config.tags[i].tag;\n\
  }\n\
\n\
  // rebuilt target tag select\n\
  var controlForm = document.getElementById(\"control\");\n\
  var rfid = control.elements.rfid;\n\
  // remove all tag options leaving none & next\n\
  while(rfid.options.length > 2) {\n\
    rfid.options.remove(rfid.options.length-1);\n\
  }\n\
  // add tags from config\n\
  for(var i=0; i<config.tags.length; i++) {\n\
    if(config.tags[i].tagName!=\"\" && config.tags[i].tag!=\"\") {\n\
      var o = document.createElement(\"option\");\n\
      o.value = config.tags[i].tag;\n\
      o.text = config.tags[i].tagName;\n\
      rfid.options.add(o);\n\
    }\n\
  }\n\
  \n\
} \n\
\n\
function buttonDisable(disable) {\n\
  var inputs = document.getElementsByTagName(\"input\");\n\
  for(var i=0; i<inputs.length; i++) {\n\
    var input = inputs.item(i);\n\
    if(input.type==\"submit\") {\n\
      input.disabled = disable;\n\
    }\n\
  }\n\
  inputs = document.getElementsByTagName(\"button\");\n\
  for(i=0; i<inputs.length; i++) {\n\
    var input = inputs.item(i);\n\
    input.disabled = disable;\n\
  }\n\
}\n\
\n\
"};

const char* mapFile(String file) {
  if(file=="/index.html") return index_html;
  if(file=="/style.css") return style_css;
  if(file=="/app.js") return app_js;
  return NULL;
}

#endif
