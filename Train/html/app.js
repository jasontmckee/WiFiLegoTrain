function getData(url,callback) {
  var xhr = new XMLHttpRequest();
  xhr.addEventListener( "load", function(event) {
    document.getElementById("response").innerHTML = JSON.stringify(JSON.parse(event.target.responseText),null,2);
    if(callback) {
      callback(JSON.parse(event.target.responseText));
    }
    buttonDisable(false);
  });
  xhr.addEventListener( "error", function( event ) {
    alert('Error occurred loading ' + url);
    buttonDisable(false);
  });
  buttonDisable(true);
  xhr.open("GET", url, true);
  xhr.send();
}

function postData(url,formData,callback) {
  var xhr = new XMLHttpRequest();
  
  xhr.addEventListener( "load", function(event) {
    document.getElementById("response").innerHTML = JSON.stringify(JSON.parse(event.target.responseText),null,2);
    if(callback) {
      callback(JSON.parse(event.target.responseText));
    }
    buttonDisable(false);
  });
  xhr.addEventListener( "error", function( event ) {
    alert('Error occurred loading ' + url );
    buttonDisable(false);
  });
  
  buttonDisable(true);
  xhr.open("POST",url);
  xhr.send(formData);
}

// register senders
window.addEventListener("load",function() {
  // pull config
  getData("/api/config",updateConfigForm);
  
  // setup control form handler
  var controlForm = document.getElementById("control");
  controlForm.addEventListener("submit", function(event) {
    // if rfid is blank, remove it
    var formData = new FormData(controlForm);
    console.log("rfid=" + controlForm.elements["rfid"].value);
    if(formData.get("rfid")=="") {
      formData.delete("rfid");
    }
    if(event.submitter.value=="Stop") {
      console.log("Stop");
      formData.delete("speed");
      formData.append("speed",0);
      formData.delete("dir");
      formData.append("dir","F");
    }
    postData("/api/control",formData);
    event.preventDefault();
  } );
  
  // setup config form handler
  var configForm = document.getElementById("config");
  configForm.addEventListener("submit", function(event) {
    postData("/api/config",new FormData(configForm),updateConfigForm);
    event.preventDefault();
  } );
  
});

function updateConfigForm(config) {
  document.title = config.name;
  document.getElementById("title").innerHTML = document.title;
  
  var configForm = document.getElementById("config");
  configForm.elements.name.value = config.name;
  configForm.elements.batteryEmpty.value = config.batteryEmpty;
  configForm.elements.batteryEmpty.nextElementSibling.innerText = config.batteryEmpty;
  configForm.elements.batteryFull.value = config.batteryFull;
  configForm.elements.batteryFull.nextElementSibling.innerText = config.batteryFull;
  configForm.elements.motorMin.value = config.motorMin;
  configForm.elements.motorMin.nextElementSibling.innerText = config.motorMin;
  configForm.elements.motorMax.value = config.motorMax;
  configForm.elements.motorMax.nextElementSibling.innerText = config.motorMax;
  configForm.elements.activeBrake.value = config.activeBrake;
  configForm.elements.brakeTimeMS.value = config.brakeTimeMS;
  configForm.elements.brakeTimeMS.nextElementSibling.innerText = config.brakeTimeMS;
  configForm.elements.displayRaw.value = config.displayRaw;
  configForm.elements.dynamicPower.value = config.dynamicPower;
  configForm.elements.beep.value = config.beep;
  configForm.elements.beepMS.value = config.beepMS;
  configForm.elements.beepMS.nextElementSibling.innerText = config.beepMS;
  
  // populate tag fields
  for(var i=0; i<config.tags.length; i++) {
    document.getElementById("tagName" + i).value = config.tags[i].tagName;
    document.getElementById("tag" + i).value = config.tags[i].tag;
  }

  // rebuilt target tag select
  var controlForm = document.getElementById("control");
  var rfid = control.elements.rfid;
  // remove all tag options leaving none & next
  while(rfid.options.length > 2) {
    rfid.options.remove(rfid.options.length-1);
  }
  // add tags from config
  for(var i=0; i<config.tags.length; i++) {
    if(config.tags[i].tagName!="" && config.tags[i].tag!="") {
      var o = document.createElement("option");
      o.value = config.tags[i].tag;
      o.text = config.tags[i].tagName;
      rfid.options.add(o);
    }
  }
  
} 

function buttonDisable(disable) {
  var inputs = document.getElementsByTagName("input");
  for(var i=0; i<inputs.length; i++) {
    var input = inputs.item(i);
    if(input.type=="submit") {
      input.disabled = disable;
    }
  }
  inputs = document.getElementsByTagName("button");
  for(i=0; i<inputs.length; i++) {
    var input = inputs.item(i);
    input.disabled = disable;
  }
}

