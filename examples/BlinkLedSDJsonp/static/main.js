/*
 * Copyright 2010 Ovidiu Predescu <ovidiu@gmail.com>
 * Date: December 2010
 *
 * Modified by Dario Di Maio
 * Modified by STeXE89
 */
String.prototype.trim = function () {
    return this.replace(/^\s*/, "").replace(/\s*$/, "");
};

function Button(elem) {
    var btn = this;
    this.elem = elem;
    this.setEnabled(false);
    this.elem.click(function(e) {
	e.preventDefault();
	btn.clickHandler(btn, e);
    });
};

Button.prototype.setEnabled = function(yesno) {
    this.enabled = yesno;
    this.elem.attr('class', function(idx, val) {
	var newClass = yesno ? "on" : "off";
	return newClass;
    });
};

Button.prototype.isEnabled = function() { return this.enabled; };

// Send a Get request and do nothing, Arduino will parse the URL
// and use it to toggle the LED
Button.prototype.clickHandler = function(btn, e) {
    var url = btn.elem.attr('href');
	$.get(url, function(json) {
		}, "jsonp");   
	   
};

// Get the status from the Arduino
function ledStatus(btn, url) {
	$.get(url, function(json) {
		btn.setEnabled(parseInt(json.light[0].status));
		window.setTimeout(ledStatus, 500, btn, url);
		}, "jsonp");

};

// Start the ledStatus function
$(document).ready(
    function() {
	var lightBulb = new Button($("#lightbulb"));
	ledStatus(lightBulb, "jsonp");
    });
