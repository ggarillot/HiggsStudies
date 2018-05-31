#!/usr/bin/env python

import os

class Params :
	def __init__(self) :
		self.outputFileName = 'test.root'
		self.processID = 0
		self.sqrtS = 250
		self.yCut = 0
		self.valueAngle = 0
		self.alphaAngle = 0
		self.maxRecordNumber = 0

def launch(a , files) :

	fileList = ''
	for name in files :
		fileList += name + ' '

	pid = os.getpid()

	xmlFileName = str(pid) + '.xml'

	xml = '''
<marlin>

	<execute>
		<processor name="HiggsProcessor"/>
	</execute>

	<global>
		<parameter name="LCIOInputFiles">''' + fileList + '''</parameter>
		<parameter name="MaxRecordNumber" value="''' + str(a.maxRecordNumber) + '''"/>
		<!--parameter name="SkipNEvents" value="18000" /-->
		<parameter name="SupressCheck" value="false" />
		<!--parameter name="GearXMLFile"> gear_ldc.xml </parameter-->
		<parameter name="Verbosity" options="DEBUG0-4,MESSAGE0-4,WARNING0-4,ERROR0-4,SILENT"> MESSAGE </parameter> 
 	</global>

 	<processor name="HiggsProcessor" type="HiggsProcessor">
 	 	<parameter name="ProcessID" type="float">''' + str(a.processID) + '''</parameter>
 		<parameter name="sqrtS" type="float">''' + str(a.sqrtS) + '''</parameter>
 		<parameter name="yCut" type="float">''' + str(a.yCut) + '''</parameter>
  		<parameter name="valueAngle" type="float">''' + str(a.valueAngle) + '''</parameter>
 		<parameter name="alphaAngle" type="float">''' + str(a.alphaAngle) + '''</parameter>
		<parameter name="RootFileName" type="string" >''' + a.outputFileName + '''</parameter>
	</processor>

</marlin>'''

	xmlFile = open(xmlFileName , 'w')
	xmlFile.write(xml)
	xmlFile.close()


	os.system('Marlin ' + xmlFileName)
	os.system('rm ' + xmlFileName)



if __name__ == '__main__' :

	processID = '106485'
	yCut = 0.003
	os.environ["MARLIN_DLL"] = '/home/guillaume/HiggsStudies/lib/libHiggsStudies.so'

	#dir = '/home/garillot/files/ILD/dst/qqh'
	#dir = '/home/garillot/files/ILD/new/Reconstructed/106485'
	dir = '/home/guillaume/files/ILD/v02-00/Reconstructed/' + processID

	print ('Searching files in ' + dir)

	#list files
	fileList = []

	for fileName in os.listdir(dir) :
		if processID + '.' in fileName :
			fileList.append(dir + '/' + fileName)

	a = Params()
	a.processID = processID
	a.yCut = yCut
	a.outputFileName = 'test' + processID + '.root'

	launch(a,fileList)


