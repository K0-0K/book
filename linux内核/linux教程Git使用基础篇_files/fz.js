document.body.oncopy = function () { 
	setTimeout( function () { 
		var text = clipboardData.getData("text");
		if (text) { 
			text = text + "\r\n文章来自中国建站："+location.href; clipboardData.setData("text", text);
		} 
	}, 100 ) 
}
