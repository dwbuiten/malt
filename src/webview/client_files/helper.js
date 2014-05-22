/********************************************************************/
var MATT_POWER_PS  = ['&nbsp;','K','M','G','T','P'];
var MATT_POWER_NPS = [' ','K','M','G','T','P'];

/********************************************************************/
function MattHelper()
{
}

/********************************************************************/
/** Short helper to convert values to human readable format **/	
MattHelper.prototype.humanReadableValue = function(value,unit,protectedSpace)
{
	var power = 0;
	while (value >= 1024)
	{
		power++;
		value /= 1024;
	}

	var res;
	if (protectedSpace)
		res = value.toFixed(1) + " " + MATT_POWER_PS[power] + unit;
	else
		res = value.toFixed(1) + " " + MATT_POWER_NPS[power] + unit;

	return res;
}

/********************************************************************/
/** Help to check values **/
MattHelper.prototype.assert = function(condition,message)
{
	if (!condition)
		throw Error("Assert failed" + (typeof message !== "undefined" ? ": " + message : ""));
}

//setup global instance
var mattHelper = new MattHelper();
