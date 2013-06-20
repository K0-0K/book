/*
 * jQuery SyntaxHighlighter Plugin
 * version: 1.1
 * @requires jQuery v1.2.2 or later
 *
 * Copyright (c) 2008 AlloVince
 * Examples at: http://allo.ave7.net/JQuery_with_SyntaxHighlighter
 * Licensed under the MIT License:
 *   http://www.opensource.org/licenses/mit-license.php
 *
 */
if(jQuery) (function($){

$.extend($, {
	SyntaxHighlighter: function (option) {
		//原有设置
		var highlighter_name = option.name!= undefined ?  option.name : "SyntaxHighlighter";
		var showGutter = option.showGutter!= undefined ? option.showGutter : true;
		var showControls = option.showControls!= undefined ? option.showControls : false;
		var collapseAll = option.collapseAll!= undefined ? option.collapseAll :false;
		var firstLine = option.firstLine ? option.firstLine : 1;
		var showColumns = option.showColumns!= undefined ? option.showColumns : false;
		//插件设置
		if(typeof(option) == "string") {
			var dir = option;
		}
		if(typeof(option) == "object") {
			var dir = option.dir;
		}
		var apptoall = option.apptoall != undefined ? option.apptoall : true;
		var autofind = option.autofind != undefined ? option.autofind : true;
		var jspath = option.jspath ? option.jspath : dir + "Scripts/";
		var csspath = option.csspath ? option.csspath : dir + "Styles/";
		var swfpath = option.swfpath  ? option.swfpath : dir + "Scripts/";
		var highlighter = {
			cpp:{
				alias:"c,c++",
				has:false
			},
			csharp:{
				alias:"c#,c-sharp",
				has:false
			},
			css:{
				has:false
			},
			delphi:{
				alias:"pascal",
				has:false
			},
			java:{
				has:false
			},
			jscript: {
				alias:"js,javascript",
				has:false
			},
			php:{
				has:false
			},
			python:{
				alias:"py",
				has:false
			},
			ruby:{
				alias:"rails,ror",
				has:false
			},
			sql:{
				has:false
			},
			vb:{
				alias:"vb.net",
				has:false
			},
			xml:{
				alias:"html,xhtml,xslt",
				has:false
			}
		}
		var highlighter_count = 0;
		if(autofind == true) {
			//自动寻找条件1:有class
			var finds = "pre[class],textarea[class]";
		}
		else {
			var finds = "pre[name='" + highlighter_name + "'][class],textarea[name='" + highlighter_name + "'][class]";
		}
		//计数
		$(finds).each(function(){
			var code_type = $(this).attr("class");
			code_type = code_type.split(":");
			code_type = code_type[0];
			if($(this).css("display") == "none") {
				$(this).attr("name",highlighter_name + '_lighted');
				//continue;
			}
			//自动寻找条件2:未定义name
			if ($(this).attr("name") == undefined || $(this).attr("name") == highlighter_name) {
				for (var types in highlighter) {
					if (types == code_type) {
						if (highlighter[types].has == false) {
							highlighter_count++;
						}
						highlighter[types].has = true;
						$(this).attr("name", highlighter_name);
						break;
					}
					//别名
					else if (highlighter[types].alias) {
						var alias = highlighter[types].alias.split(",");
						for (var i = 0; i < alias.length; i++) {
							if (code_type == alias[i]) {
								if (highlighter[types].has == false) {
									highlighter_count++;
								}
								highlighter[types].has = true;
								$(this).attr("name", highlighter_name);
								break;
							}
						}
					}
				}
				
			}
		});
		
		//首字母大写函数 From http://tech.karbassi.com/2007/10/08/javascript-ucfirst/
		String.prototype.ucfirst = function() {
		   var x = this.split(/\s+/g);
		   for(var i = 0; i < x.length; i++) {
		      var parts = x[i].match(/(\w)(\w*)/);
		      x[i] = parts[1].toUpperCase() + parts[2].toLowerCase();
		   }
		   return x.join(' ');
		};
		if(highlighter_count > 0) {
			$.getScript(jspath + "shCore.js",function(){
				eval(this);
				$("head").append("<link rel=\"stylesheet\" type=\"text/css\" media=\"screen\" href=\"" + csspath + "SyntaxHighlighter.css\" />");
				var i = 0;
				for(var types in highlighter) {
					if(highlighter[types].has == true) {
						//文件首字母大写
						var jsfile = jspath + "shBrush" + types.ucfirst() + ".js";
						//利用JS反射动态读取所需要JS文件并执行
						$.getScript(jsfile,function(){
							eval(this);
							i++;
							if(i == highlighter_count) {
								//在最后一次读取后执行高亮
								dp.SyntaxHighlighter.ClipboardSwf = swfpath + 'clipboard.swf';
								if(apptoall == false) {
									dp.SyntaxHighlighter.HighlightAll(highlighter_name);
								}
								if(apptoall == true) {
									dp.SyntaxHighlighter.HighlightAll(highlighter_name,showGutter,showControls,collapseAll,firstLine,showColumns);
								}
							}
						});
						
					}
				}
			});
		}

	}
})

})(jQuery);
