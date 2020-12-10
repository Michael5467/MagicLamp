var slider1 = document.getElementById("BrightnesSlider");
var output1 = document.getElementById("BrightnesValue");
output1.innerHTML = slider1.value;

slider1.oninput = function() {
    output1.innerHTML = this.value;
    const http = new XMLHttpRequest();
    http.open("POST", )
}

var slider2 = document.getElementById("SpeedSlider");
var output2 = document.getElementById("SpeedValue");
output2.innerHTML = slider2.value;

slider2.oninput = function() {
    output2.innerHTML = this.value;
}

var slider3 = document.getElementById("DensitySlider");
var output3 = document.getElementById("DensityValue");
output3.innerHTML = slider3.value;

slider3.oninput = function() {
    output3.innerHTML = this.value;
}
