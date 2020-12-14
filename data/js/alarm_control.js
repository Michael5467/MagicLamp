var allCheckBoxElem = document.getElementsByClassName("item_checkbox")
// console.log(allCheckBoxElem);
for (var i = 0; i < allCheckBoxElem.length; i++) {
    // console.log(allCheckBoxElem[i]);
    allCheckBoxElem[i].addEventListener("click", clickOnFlag);
    disTimeElem(allCheckBoxElem[i].value + "Time", !allCheckBoxElem[i].checked);
}

function clickOnFlag(e) {
    // // console.log(e)
    // // console.log(e.target);
    // var currTimeElem = document.getElementById(e.target.value + "Time");
    // // console.log(currTimeElem);
    // currTimeElem.disabled = !e.target.checked;

    disTimeElem(e.target.value + "Time", !e.target.checked);
}

function disTimeElem(name, flag) {
    var currTimeElem = document.getElementById(name);
    currTimeElem.disabled = flag;
}
