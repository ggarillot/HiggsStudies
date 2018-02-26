#!/usr/bin/env python

import os

class Params :
	def __init__(self) :
		self.outputFileName = 'test.root'
		self.dMin = 375

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
		<!--parameter name="MaxRecordNumber" value="5"/-->
		<!--parameter name="SkipNEvents" value="18000" /-->
		<parameter name="SupressCheck" value="false" />
		<!--parameter name="GearXMLFile"> gear_ldc.xml </parameter-->
		<parameter name="Verbosity" options="DEBUG0-4,MESSAGE0-4,WARNING0-4,ERROR0-4,SILENT"> MESSAGE </parameter> 
 	</global>

 	<processor name="HiggsProcessor" type="HiggsProcessor">
 		<parameter name="dMin" type="float">''' + str(a.dMin) + '''</parameter>
		<parameter name="RootFileName" type="string" >''' + a.outputFileName + '''</parameter>
	</processor>

</marlin>'''

	xmlFile = open(xmlFileName , 'w')
	xmlFile.write(xml)
	xmlFile.close()


	os.system('Marlin ' + xmlFileName)
	os.system('rm ' + xmlFileName)



if __name__ == '__main__' :

	os.environ["MARLIN_DLL"] = '/home/garillot/Code/HiggsStudies/lib/libHiggsStudies.so'
	
	dir = '/home/garillot/files/ILD/dst/qqh'
	#dir = '/home/garillot/files/ILD/dst/zz'
	#dir = '/home/garillot/files/ILD/dst/ww'


	print ('Searching files in ' + dir)

	#list files
	fileList = []

	for fileName in os.listdir(dir) :
		if 'mILD_o2_v05.E250-TDR_ws.Pqqh.Gwhizard-1_95.eL.pR.I106485.' in fileName :
		#if 'mILD_o2_v05.E250-TDR_ws.P4f_zz_h.Gwhizard-1_95.eL.pR.I106573.' in fileName :
		#if 'mILD_o2_v05.E250-TDR_ws.P4f_ww_h.Gwhizard-1_95.eL.pR.I106551.' in fileName :
			fileList.append(dir + '/' + fileName)

	#print 'File List :'
	#print fileList

	a = Params()
	#a.dMin = 375
	a.outputFileName = "qqH.root"
	#a.outputFileName = "ZZ.root"
	#a.outputFileName = "WW.root"

	launch(a,fileList)

