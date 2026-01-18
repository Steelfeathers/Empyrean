Scriptname Empyrean_DLLFunctions

;/
Returns the version of the Empyrean DLL, formatted as an array of three integers:
Example:
	Int[] iResponse = Empyrean_DLLFunctions.GetVersion()
	If (iResponse)

		Int iMajor = iResponse[0]
		Int iMinor = iResponse[1]
		Int iPatch = iResponse[2]
	EndIf
/;
Int[] Function GetVersion() Global Native