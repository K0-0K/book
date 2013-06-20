// Anthem.js
// Updated Mar 30, 2007
// Anthem.Manager.GetScripts: false

function Anthem_Encode(s){
	if (typeof encodeURIComponent == "function") {
		// Use JavaScript built-in function
		// IE 5.5+ and Netscape 6+ and Mozilla
		return encodeURIComponent(s);
	} else {
		// Need to mimic the JavaScript version
		// Netscape 4 and IE 4 and IE 5.0
		return encodeURIComponentNew(s);
	}
}

// Primarily used by Anthem.Manager to add an onsubmit event handler
// when validators are added to a page during a callback.
function Anthem_AddEvent(control, eventType, functionPrefix) {
    var ev;
    eval("ev = control." + eventType + ";");
    if (typeof(ev) == "function") {
        ev = ev.toString();
        ev = ev.substring(ev.indexOf("{") + 1, ev.lastIndexOf("}"));
    }
    else {
        ev = "";
    }
    var func;
    if (navigator.appName.toLowerCase().indexOf('explorer') > -1) {
        func = new Function(functionPrefix + " " + ev);
    }
    else {
        func = new Function("event", functionPrefix + " " + ev);
    }
    eval("control." + eventType + " = func;");
}

function Anthem_GetXMLHttpRequest() {
	if (window.XMLHttpRequest) {
		return new XMLHttpRequest();
	} else {
		if (window.Anthem_XMLHttpRequestProgID) {
			return new ActiveXObject(window.Anthem_XMLHttpRequestProgID);
		} else {
			var progIDs = ["Msxml2.XMLHTTP.6.0", "Msxml2.XMLHTTP.5.0", "Msxml2.XMLHTTP.4.0", "MSXML2.XMLHTTP.3.0", "MSXML2.XMLHTTP", "Microsoft.XMLHTTP"];
			for (var i = 0; i < progIDs.length; ++i) {
				var progID = progIDs[i];
				try {
					var x = new ActiveXObject(progID);
					window.Anthem_XMLHttpRequestProgID = progID;
					return x;
				} catch (e) {
				}
			}
		}
	}
	return null;
}

// This array is used to keep track of clientCallBack functions when using
// an IOFrame to handle callbacks.
var callbackFrames = new Array();

// This function is called by the onload event of the IOFrame after the
// callback response is received. The function parses the response, updates
// the page, and invokes the clientCallBack function.
function Anthem_HandleIOFrameResponse(frameid) {
    var iframe = document.getElementById(frameid);
    if (iframe != null) {
        var doc = Anthem_ExtractIFrameDocument(iframe);
        if (doc.getElementsByTagName("textarea").length > 0) {
            // Extract the response
            var response = { responseText: doc.getElementById("response").value.replace(/<\/anthemarea>/, "</textarea>") };
            if (typeof(Anthem_DebugResponseText) == "function") {
                Anthem_DebugResponseText(response.responseText); 
            }
            // Parse the response
            var result = Anthem_GetResult(response); 
            if (result.error) { 
                if (typeof(Anthem_DebugError) == "function") {
                    Anthem_DebugError(result.error); 
                }
                if (typeof(window.Anthem_Error) == "function") { 
                    Anthem_Error(result); 
                } 
            } 
            // Update the page 
            Anthem_UpdatePage(result); 
            // Run the client scripts
            Anthem_EvalClientSideScript(result); 
            // Run the custom post callback function
            for (var index in callbackFrames) {
                var frame = callbackFrames[index];
                if (frame != null && frame.id == frameid) {
                    callbackFrames.splice(index, 1);
                    frame.clientCallBack(result, frame.clientCallBackArg);
                    break;
                }
            }
            // Run the common post callback function
            if (typeof(window.Anthem_PostCallBack) == "function") { 
                Anthem_PostCallBack(); 
            } 
        }
        setTimeout("document.body.removeChild(document.getElementById(\"" + frameid + "\"))", 10);
    }
}

// Returns the iframe document
function Anthem_ExtractIFrameDocument(iFrameEl) {
  var doc = null;
  if (iFrameEl.contentDocument) { // For NS6
    doc = iFrameEl.contentDocument;
  } else if (iFrameEl.contentWindow) { // For IE5.5 and IE6
    doc = iFrameEl.contentWindow.document;
  } else if (iFrameEl.document) { // For IE5
    doc = iFrameEl.document;
  } else {
    //alert("Error: could not find iFrame document");
    return null;
  }
  return doc;
}

// Returns the form that is posted back using AJAX
function Anthem_GetForm() {
    var form = document.getElementById(Anthem_FormID);
    return form;
}

// Returns the URL for callbacks
function Anthem_GetCallBackUrl() {
    var form = Anthem_GetForm();
    var action = form.action + (form.action.indexOf('?') == -1 ? "?" : "&") + "Anthem_CallBack=true";
    return action;
}

function Anthem_CallBack(url, target, id, method, args, clientCallBack, clientCallBackArg, includeControlValuesWithCallBack, updatePageAfterCallBack) {
	if (typeof(window.Anthem_PreCallBack) == "function") {
		var preCallBackResult = Anthem_PreCallBack();
		if (!(typeof preCallBackResult == "undefined" || preCallBackResult)) {
			if (typeof(window.Anthem_CallBackCancelled) == "function") {
				Anthem_CallBackCancelled();
			}
			return null;
		}
	}
    var encodedData = "";
    if (target == "Page") {
        encodedData += "&Anthem_PageMethod=" + method;
    } else if (target == "MasterPage") {
        encodedData += "&Anthem_MasterPageMethod=" + method;
    } else if (target == "Control") {
        encodedData += "&Anthem_ControlID=" + id.split(":").join("_");
        encodedData += "&Anthem_ControlMethod=" + method;
    }
	if (args) {
		for (var argsIndex = 0; argsIndex < args.length; ++argsIndex) {
			if (args[argsIndex] instanceof Array) {
				for (var i = 0; i < args[argsIndex].length; ++i) {
					encodedData += "&Anthem_CallBackArgument" + argsIndex + "=" + Anthem_Encode(args[argsIndex][i]);
				}
			} else {
				encodedData += "&Anthem_CallBackArgument" + argsIndex + "=" + Anthem_Encode(args[argsIndex]);
			}
		}
	}
	
	if (updatePageAfterCallBack) {
		encodedData += "&Anthem_UpdatePage=true";
	}
	
	// Anthem will normally use an XmlHttpRequest to communicate with the server. 
	// But if an Anthem.FileUpload control is discovered on the page, then Anthem
	// will use a hidden IFRAME instead. This hidden IFRAME is often called an IOFrame
	// by AJAX library authors, so that is the name we use here.
	var useIOFrame = false;
	
	// Scan the controls on the form and extract their values.
	if (includeControlValuesWithCallBack) {
		var form = Anthem_GetForm();
		if (form != null) {
			for (var elementIndex = 0; elementIndex < form.length; ++elementIndex) {
				var element = form.elements[elementIndex];
				if (element.name) {
					var elementValue = null;
					if (element.nodeName.toUpperCase() == "INPUT") {
						var inputType = element.getAttribute("type").toUpperCase();
						if (inputType == "TEXT" || inputType == "PASSWORD" || inputType == "HIDDEN") {
							elementValue = element.value;
						} else if (inputType == "CHECKBOX" || inputType == "RADIO") {
							if (element.checked) {
								elementValue = element.value;
							}
						} else if (inputType == "FILE") {
						    // If the FILE element has a value (the path to the file), then an
						    // IOFrame will be used to handle the callback.
						    useIOFrame = useIOFrame | !(element.value == null || element.value.length == 0);
						}
					} else if (element.nodeName.toUpperCase() == "SELECT") {
						if (element.multiple) {
							elementValue = [];
							for (var i = 0; i < element.length; ++i) {
								if (element.options[i].selected) {
									elementValue.push(element.options[i].value);
								}
							}
						} else if (element.length == 0) {
						    elementValue = null;
						} else {
							elementValue = element.value;
						}
					} else if (element.nodeName.toUpperCase() == "TEXTAREA") {
						elementValue = element.value;
					}
					if (elementValue instanceof Array) {
						for (var i = 0; i < elementValue.length; ++i) {
							encodedData += "&" + element.name + "=" + Anthem_Encode(elementValue[i]);
						}
					} else if (elementValue != null) {
						encodedData += "&" + element.name + "=" + Anthem_Encode(elementValue);
					}
				}
			}
			// ASP.NET 1.1 won't fire any events if neither of the following
			// two parameters are not in the request so make sure they're
			// always in the request.
			if (typeof form.__VIEWSTATE == "undefined") {
				encodedData += "&__VIEWSTATE=";
			}
			if (typeof form.__EVENTTARGET == "undefined") {
				encodedData += "&__EVENTTARGET=";
			}
		}
	}
	
	if (encodedData.length > 0) {
	    encodedData = encodedData.substring(1);
	}
	if (typeof(Anthem_DebugRequestText) == "function") {
	    Anthem_DebugRequestText(encodedData.split("&").join("\n&"));
	}

    // Send the callback request to the server. Use an IOFrame if there is a file upload,
    // otherwise use an XmlHttpRequest.
    if (useIOFrame) {
        // To allow multiple requests at the same time, all of the Anthem parameters are 
        // passed to the server via the querystring
        var action = Anthem_GetCallBackUrl();
        action = action + "&Anthem_IOFrame=true";
        if (updatePageAfterCallBack) {
            action = action + "&Anthem_UpdatePage=true";
        }
        
        // We could generate an anonymous function on the fly to handle the clientCallBack
        // and assign that to the iframe onload event (in fact this is how XmlHttpRequests are
        // handled). But that makes it very hard to debug the callback response. Instead
        // we will stuff the clientCallBack function and args into an array and then hard code
        // the onload event handler. The handler will find the appropriate callback info in
        // the array and handle the clientCallBack.
        var id = "f" + new String(Math.floor(9999 * Math.random())); // Generate frame number
        if (typeof(clientCallBack) == "function") {
            var frame = { "id":id, "clientCallBack":clientCallBack, "clientCallBackArg":clientCallBackArg };
            callbackFrames.push(frame);
        }
        
        // Create a new, invisible iframe to handle the io.
        var ioframe = null;
        if (window.ActiveXObject) {
            ioframe = document.createElement("<iframe name=\"" + id + "\" id=\"" + id + "\" onload=\"Anthem_HandleIOFrameResponse('" + id + "');\"/>");
        } else {
            ioframe = document.createElement("iframe");
            ioframe.id = id;
            ioframe.name = id;
            ioframe.onload = function (){ Anthem_HandleIOFrameResponse(id); }
        }
        ioframe.style.visibility = "hidden";
        ioframe.style.height = "1px";
        document.body.appendChild(ioframe);

        // Submit this form in the hidden iframe
        var theForm = Anthem_GetForm(); 
        var tempActionUri = theForm.action; 
        theForm.action = action; 
        theForm.target = id;
        try { 
            theForm.submit(); 
        } catch (e) {
            result = { "value": null, "error": e.message };
		    if (typeof(Anthem_DebugError) == "function") {
		        Anthem_DebugError(e.name + ": " + e.message + " (" + e.number + ")");
		    }
		    if (typeof(window.Anthem_Error) == "function") {
			    Anthem_Error(result);
		    }            
        }

        // Restore the form 
        theForm.target = ""; 
        theForm.action = tempActionUri;
        
    } else {
    
	    var x = Anthem_GetXMLHttpRequest();
	    var result = null;
	    if (!x) {
		    result = { "value": null, "error": "NOXMLHTTP" };
		    if (typeof(Anthem_DebugError) == "function") {
		        Anthem_DebugError(result.error);
		    }
		    if (typeof(window.Anthem_Error) == "function") {
			    Anthem_Error(result);
		    }
		    if (typeof(clientCallBack) == "function") {
			    clientCallBack(result, clientCallBackArg);
		    }
		    return result;
	    }
	    var action = Anthem_GetCallBackUrl();
	    x.open("POST", url ? url : action, clientCallBack ? true : false);
	    x.setRequestHeader("Content-Type", "application/x-www-form-urlencoded; charset=utf-8");
	    x.setRequestHeader("Accept-Encoding", "gzip, deflate");
	    if (typeof(clientCallBack) == "function") {
		    x.onreadystatechange = function() {
			    if (x.readyState != 4) {
				    return;
			    }
			    if (typeof(Anthem_DebugResponseText) == "function") {
			        Anthem_DebugResponseText(x.responseText);
			    }
			    result = Anthem_GetResult(x);
			    if (result.error) {
			        if (typeof(Anthem_DebugError) == "function") {
				        Anthem_DebugError(result.error);
				    }
				    if (typeof(window.Anthem_Error) == "function") {
					    Anthem_Error(result);
				    }
			    }
			    if (updatePageAfterCallBack) {
				    Anthem_UpdatePage(result);
			    }
			    Anthem_EvalClientSideScript(result);
			    clientCallBack(result, clientCallBackArg);
			    x = null;
			    if (typeof(window.Anthem_PostCallBack) == "function") {
				    Anthem_PostCallBack();
			    }
		    }
	    }
	    x.send(encodedData);
	    if (typeof(clientCallBack) != "function") {
	        if (typeof(Anthem_DebugResponseText) == "function") {
		        Anthem_DebugResponseText(x.responseText);
		    }
		    result = Anthem_GetResult(x);
		    if (result.error) {
		        if (typeof(Anthem_DebugError) == "function") {
			        Anthem_DebugError(result.error);
			    }
			    if (typeof(window.Anthem_Error) == "function") {
				    Anthem_Error(result);
			    }
		    }
		    if (updatePageAfterCallBack) {
			    Anthem_UpdatePage(result);
		    }
		    Anthem_EvalClientSideScript(result);
		    if (typeof(window.Anthem_PostCallBack) == "function") {
			    Anthem_PostCallBack();
		    }
	    }
    }	
	return result;
}

function Anthem_GetResult(x) {
	var result = { "value": null, "error": null };
	var responseText = x.responseText;
	try {
		result = eval("(" + responseText + ")");
	} catch (e) {
		if (responseText.length == 0) {
			result.error = "NORESPONSE";
		} else {
			result.error = "BADRESPONSE";
			result.responseText = responseText;
		}
	}
	return result;
}

function Anthem_SetHiddenInputValue(form, name, value) {
    var input = null;
    if (form[name]) {
        input = form[name];
    } else {
        input = document.createElement("input");
        input.setAttribute("name", name);
        input.setAttribute("type", "hidden");
    }
    input.setAttribute("value", value);
    var parentElement = input.parentElement ? input.parentElement : input.parentNode;
    if (parentElement == null) {
        form.appendChild(input);
        form[name] = input;
    }
}

function Anthem_RemoveHiddenInput(form, name) {
    var input = form[name];
    if (input != null && typeof(input) != "undefined") {
      var parentElement = input.parentElement ? input.parentElement : input.parentNode;
      if (parentElement != null) {
          form[name] = null;
          parentElement.removeChild(input);
      }
    }
}

function Anthem_FireEvent(eventTarget, eventArgument, clientCallBack, clientCallBackArg, includeControlValuesWithCallBack, updatePageAfterCallBack) {
	var form = Anthem_GetForm();
	Anthem_SetHiddenInputValue(form, "__EVENTTARGET", eventTarget);
	Anthem_SetHiddenInputValue(form, "__EVENTARGUMENT", eventArgument);
	Anthem_CallBack(null, null, null, null, null, clientCallBack, clientCallBackArg, includeControlValuesWithCallBack, updatePageAfterCallBack);
	form.__EVENTTARGET.value = "";
	form.__EVENTARGUMENT.value = "";
}

function Anthem_UpdatePage(result) {
	var form = Anthem_GetForm();
	if (result.viewState) {
		Anthem_SetHiddenInputValue(form, "__VIEWSTATE", result.viewState);
	}
	if (result.viewStateEncrypted) {
		Anthem_SetHiddenInputValue(form, "__VIEWSTATEENCRYPTED", result.viewStateEncrypted);
	}
	if (result.eventValidation) {
		Anthem_SetHiddenInputValue(form, "__EVENTVALIDATION", result.eventValidation);
	}
	if (result.controls) {
		for (var controlID in result.controls) {
			var containerID = "Anthem_" + controlID.split("$").join("_") + "__";
			var control = document.getElementById(containerID);
			if (control) {
				control.innerHTML = result.controls[controlID];
				if (result.controls[controlID] == "") {
					control.style.display = "none";
				} else {
					control.style.display = "";
				}
			}
		}
	}
	if (result.pagescript) {
	    Anthem_LoadPageScript(result, 0);
	}
}

// Load each script in order and wait for each one to load before proceeding
function Anthem_LoadPageScript(result, index) {
    if (index < result.pagescript.length) {
		try {
		    var script = document.createElement('script');
		    script.type = 'text/javascript';
		    if (result.pagescript[index].indexOf('src=') == 0) {
		        script.src = result.pagescript[index].substring(4);
		    } else {
		        if (script.canHaveChildren ) {
		            script.appendChild(document.createTextNode(result.pagescript[index]));
		        } else {
		            script.text = result.pagescript[index];
		        }
		    }
		    var heads = document.getElementsByTagName('head');
		    if (heads != null && typeof(heads) != "undefined" && heads.length > 0) {
		        var head = heads[0];

		        // The order that scripts appear is important since later scripts can
		        // redefine a function. Therefore it is important to add every script
		        // to the page and in the same order that they were added on the server.
		        // On the other hand, if we just keep adding scripts the DOM will grow
		        // unnecessarily. This code scans the <head> element block and removes 
		        // previous instances of the identical script.
		        var found = false;
		        for (var child = 0; child < head.childNodes.length; child++) {
		            var control = head.childNodes[child];
		            if (typeof(control.tagName) == "string") {
		                if (control.tagName.toUpperCase() == "SCRIPT") {
		                    if (script.src.length > 0) {
		                        if (script.src == control.src) {
		                            found = true;
		                            break;
		                        }
		                    } else if (script.innerHTML.length > 0) {
		                        if (script.innerHTML == control.innerHTML) {
		                            found = true;
		                            break;
		                        }
		                    }
		                }
		            }
		        }
		        if (found) {
		            head.removeChild(control);
		        }
		        
		        // Now we append the new script and move on to the next script.
		        // Note that this is a recursive function. It stops when the
		        // index grows larger than the number of scripts.
                document.getElementsByTagName('head')[0].appendChild(script);
                if (typeof script.readyState != "undefined") {
                    script.onreadystatechange = function() {
                        if (script.readyState != "complete" && script.readyState != "loaded") {
                            return;
                        } else {
                            Anthem_LoadPageScript(result, index + 1);
                        }
                    }
                } else {
                    Anthem_LoadPageScript(result, index + 1);
                }
	        }
		} catch (e) {
		    if (typeof(Anthem_DebugError) == "function") {
		        Anthem_DebugError("Error adding page script to head. " + e.name + ": " + e.message);
		    }
		}
	}
}

function Anthem_EvalClientSideScript(result) {
	if (result.script) {
		for (var i = 0; i < result.script.length; ++i) {
			try {
				eval(result.script[i]);
			} catch (e) {
				alert("Error evaluating client-side script!\n\nScript: " + result.script[i] + "\n\nException: " + e);
			}
		}
	}
}

//Fix for bug #1429412, "Reponse callback returns previous response after file push".
//see http://sourceforge.net/tracker/index.php?func=detail&aid=1429412&group_id=151897&atid=782464
function Anthem_Clear__EVENTTARGET() {
	var form = Anthem_GetForm();
	Anthem_SetHiddenInputValue(form, "__EVENTTARGET", "");
}

function Anthem_InvokePageMethod(methodName, args, clientCallBack, clientCallBackArg) {
	Anthem_Clear__EVENTTARGET(); // fix for bug #1429412
    return Anthem_CallBack(null, "Page", null, methodName, args, clientCallBack, clientCallBackArg, true, true);
}

function Anthem_InvokeMasterPageMethod(methodName, args, clientCallBack, clientCallBackArg) {
	Anthem_Clear__EVENTTARGET(); // fix for bug #1429412
    return Anthem_CallBack(null, "MasterPage", null, methodName, args, clientCallBack, clientCallBackArg, true, true);
}

function Anthem_InvokeControlMethod(id, methodName, args, clientCallBack, clientCallBackArg) {
	Anthem_Clear__EVENTTARGET(); // fix for bug #1429412
    return Anthem_CallBack(null, "Control", id, methodName, args, clientCallBack, clientCallBackArg, true, true);
}

function Anthem_PreProcessCallBack(
    control,
    e,
    eventTarget,
    causesValidation, 
    validationGroup, 
    imageUrlDuringCallBack, 
    textDuringCallBack, 
    enabledDuringCallBack,
    preCallBackFunction,
    callBackCancelledFunction,
    preProcessOut
) {
	var valid = true;
	if (causesValidation && typeof(Page_ClientValidate) == "function") {
		valid = Page_ClientValidate(validationGroup);
	}
	if (typeof(WebForm_OnSubmit) == "function") {
	    valid = WebForm_OnSubmit();
	}
	if (valid) {
        var preCallBackResult = true;
        if (typeof(preCallBackFunction) == "function") {
	        preCallBackResult = preCallBackFunction(control, e);
        }
        if (typeof(preCallBackResult) == "undefined" || preCallBackResult) {
		    var inputType = control.getAttribute("type");
		    inputType = (inputType == null) ? '' : inputType.toUpperCase();
		    if (inputType == "IMAGE" && e != null) {
                var form = Anthem_GetForm();
                if (e.offsetX) { // IE
                    Anthem_SetHiddenInputValue(form, eventTarget + ".x", e.offsetX);
                    Anthem_SetHiddenInputValue(form, eventTarget + ".y", e.offsetY);
                } else { // FireFox + ???
                    var offset = GetControlLocation(control);
                    Anthem_SetHiddenInputValue(form, eventTarget + ".x", e.clientX - offset.x + 1 + window.pageXOffset);
                    Anthem_SetHiddenInputValue(form, eventTarget + ".y", e.clientY - offset.y + 1 + window.pageYOffset);
                }
		    }
		    if (imageUrlDuringCallBack || textDuringCallBack) {
		        var nodeName = control.nodeName.toUpperCase();
		        if (nodeName == "INPUT") {
		            if (inputType == "CHECKBOX" || inputType == "RADIO" || inputType == "TEXT") {
		                preProcessOut.OriginalText = GetLabelText(control.id);
		                SetLabelText(control.id, textDuringCallBack);
		            } else if (inputType == "IMAGE") {
		                if (imageUrlDuringCallBack) {
		                    preProcessOut.OriginalText = control.src;
		                    control.src = imageUrlDuringCallBack;
		                } else {
		                    preProcessOut.ParentElement = control.parentElement ? control.parentElement : control.parentNode;
		                    if (preProcessOut.ParentElement) {
		                        preProcessOut.OriginalText = preProcessOut.ParentElement.innerHTML;
		                        preProcessOut.ParentElement.innerHTML = textDuringCallBack;
		                    }
		                }
		            } else if (inputType == "SUBMIT" || inputType == "BUTTON") {
		                preProcessOut.OriginalText = control.value;
		                control.value = textDuringCallBack;
		            }
		        } else if (nodeName == "SELECT" || nodeName == "SPAN") {
		            preProcessOut.OriginalText = GetLabelText(control.id);
		            SetLabelText(control.id, textDuringCallBack);
		        } else {
		            preProcessOut.OriginalText = control.innerHTML;
			        control.innerHTML = textDuringCallBack;
			    }
		    }
		    // Disable the control during callback if required
		    control.disabled = (typeof(enabledDuringCallBack) == "undefined") ? false : !enabledDuringCallBack;
		    return true;
		} else {
		    // Callback cancelled
            if (typeof(callBackCancelledFunction) == "function") {
	            callBackCancelledFunction(control, e);
	        }
	        return false;
		}
    } else {
        // Validation failed
        return false;
    }
}

function Anthem_PreProcessCallBackOut() {
    // Fields
    this.ParentElement = null;
    this.OriginalText = '';
}

function Anthem_PostProcessCallBack(
    result, 
    control,
    e,
    eventTarget, 
    clientCallBack, 
    clientCallBackArg, 
    imageUrlDuringCallBack, 
    textDuringCallBack, 
    postCallBackFunction, 
    preProcessOut
) {
    if (typeof(postCallBackFunction) == "function") {
        postCallBackFunction(control, e);
    }
    // Re-enable the control if it was disabled during callback
	control.disabled = false;
    var inputType = control.getAttribute("type");
    inputType = (inputType == null) ? '' : inputType.toUpperCase();
	if (inputType == "IMAGE") {
	    var form = Anthem_GetForm();
        Anthem_RemoveHiddenInput(form, eventTarget + ".x");
        Anthem_RemoveHiddenInput(form, eventTarget + ".y");
	}
	if (imageUrlDuringCallBack || textDuringCallBack) {
	    var nodeName = control.nodeName.toUpperCase();
	    if (nodeName == "INPUT") {
	        if (inputType == "CHECKBOX" || inputType == "RADIO" || inputType == "TEXT") {
	            SetLabelText(control.id, preProcessOut.OriginalText);
	        } else if (inputType == "IMAGE") {
	            if (imageUrlDuringCallBack) {
	                control.src = preProcessOut.OriginalText;
	            } else {
	                preProcessOut.ParentElement.innerHTML = preProcessOut.OriginalText;
	            }
	        } else if (inputType == "SUBMIT" || inputType == "BUTTON") {
	            control.value = preProcessOut.OriginalText;
	        }
	    } else if (nodeName == "SELECT" || nodeName == "SPAN") {
	        SetLabelText(control.id, preProcessOut.OriginalText);
	    } else {
	        control.innerHTML = preProcessOut.OriginalText;
	    }
	}
	if (typeof(clientCallBack) == "function") {
	    clientCallBack(result, clientCallBackArg);
	}
}

function Anthem_FireCallBackEvent(
	control,
	e,
	eventTarget,
	eventArgument,
	causesValidation,
	validationGroup,
	imageUrlDuringCallBack,
	textDuringCallBack,
	enabledDuringCallBack,
	preCallBackFunction,
	postCallBackFunction,
	callBackCancelledFunction,
	includeControlValuesWithCallBack,
	updatePageAfterCallBack
) {
    // Cancel the callback if the control is disabled. Although most controls will
    // not raise their callback event if they are disabled, the LinkButton will.
    // This check is for the LinkButton. See SourceForge Patch 1639700.
    if (control.disabled) return;
	var preProcessOut = new Anthem_PreProcessCallBackOut();
	var preProcessResult = Anthem_PreProcessCallBack(
	    control, 
	    e, 
	    eventTarget,
	    causesValidation, 
	    validationGroup, 
	    imageUrlDuringCallBack, 
	    textDuringCallBack, 
	    enabledDuringCallBack, 
	    preCallBackFunction, 
	    callBackCancelledFunction, 
	    preProcessOut
	);
    if (preProcessResult) {
        var eventType = e.type;
	    Anthem_FireEvent(
		    eventTarget,
		    eventArgument,
		    function(result) {
                Anthem_PostProcessCallBack(
                    result, 
                    control, 
                    eventType,
                    eventTarget,
                    null, 
                    null, 
                    imageUrlDuringCallBack, 
                    textDuringCallBack, 
                    postCallBackFunction, 
                    preProcessOut
                );
		    },
		    null,
		    includeControlValuesWithCallBack,
		    updatePageAfterCallBack
	    );
    }
}

function AnthemListControl_OnClick(
    e,
	causesValidation,
	validationGroup,
	textDuringCallBack,
	enabledDuringCallBack,
	preCallBackFunction,
	postCallBackFunction,
	callBackCancelledFunction,
	includeControlValuesWithCallBack,
	updatePageAfterCallBack
) {
	var target = e.target || e.srcElement;
	if (target.nodeName.toUpperCase() == "LABEL" && target.htmlFor != '')
	    return;
	var eventTarget = target.id.split("_").join("$");
	Anthem_FireCallBackEvent(
	    target, 
	    e,
	    eventTarget, 
	    '', 
	    causesValidation, 
	    validationGroup, 
	    '',
	    textDuringCallBack, 
	    enabledDuringCallBack, 
	    preCallBackFunction, 
	    postCallBackFunction, 
	    callBackCancelledFunction, 
	    true, 
	    true
	);
}

// Returns the top, left control location in FireFox
function GetControlLocation(control) {
    var offsetX = 0;
    var offsetY = 0;
    var parent;
    
    for (parent = control; parent; parent = parent.offsetParent) {
        if (parent.offsetLeft) {
            offsetX += parent.offsetLeft;
        }
        if (parent.offsetTop) {
            offsetY += parent.offsetTop;
        }
    }
    
    return { x: offsetX, y: offsetY };
}

function GetLabelText(id) {
    var labels = document.getElementsByTagName('label');
    for (var i = 0; i < labels.length; i++) {
        if (labels[i].htmlFor == id) {
            return labels[i].innerHTML;
        }
    }
    return null;
}

function SetLabelText(id, text) {
    var labels = document.getElementsByTagName('label');
    for (var i = 0; i < labels.length; i++) {
        if (labels[i].htmlFor == id) {
            labels[i].innerHTML = text;
            return;
        }
    }
}

// Used by encodeURIComponentNew to mimic function encodeURIComponent in 
// IE 5.5+, Netscape 6+, and Mozilla
function utf8(wide) {
  var c, s;
  var enc = "";
  var i = 0;
  while(i<wide.length) {
    c= wide.charCodeAt(i++);
    // handle UTF-16 surrogates
    if (c>=0xDC00 && c<0xE000) continue;
    if (c>=0xD800 && c<0xDC00) {
      if (i>=wide.length) continue;
      s= wide.charCodeAt(i++);
      if (s<0xDC00 || c>=0xDE00) continue;
      c= ((c-0xD800)<<10)+(s-0xDC00)+0x10000;
    }
    // output value
    if (c<0x80) enc += String.fromCharCode(c);
    else if (c<0x800) enc += String.fromCharCode(0xC0+(c>>6),0x80+(c&0x3F));
    else if (c<0x10000) enc += String.fromCharCode(0xE0+(c>>12),0x80+(c>>6&0x3F),0x80+(c&0x3F));
    else enc += String.fromCharCode(0xF0+(c>>18),0x80+(c>>12&0x3F),0x80+(c>>6&0x3F),0x80+(c&0x3F));
  }
  return enc;
}

var hexchars = "0123456789ABCDEF";

function toHex(n) {
  return hexchars.charAt(n>>4)+hexchars.charAt(n & 0xF);
}

var okURIchars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_-";

// Mimics function encodeURIComponent in IE 5.5+, Netscape 6+, and Mozilla
function encodeURIComponentNew(s) {
  var s = utf8(s);
  var c;
  var enc = "";
  for (var i= 0; i<s.length; i++) {
    if (okURIchars.indexOf(s.charAt(i))==-1)
      enc += "%"+toHex(s.charCodeAt(i));
    else
      enc += s.charAt(i);
  }
  return enc;
}
