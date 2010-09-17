#! /Library/Frameworks/Python.framework/Versions/Current/bin/python

""" PyXDF
A python interface to XDF data files.
"""

import sys
import xml.dom.minidom

class XDF:
    def __init__(self, filename):
        self.filename = filename
        self.files_dict = dict()
        self.params_dict = dict()

    def doLoad(self):
        self.dom = xml.dom.minidom.parse(self.filename)
        self.getFileNames(self.dom.getElementsByTagName("DataFiles")[0].childNodes)

    def getFileNames(self, nodelist):
        for node in nodelist:
            if node.childNodes:
                self.files_dict[node.nodeName.strip()] = node.childNodes[0].nodeValue.strip()

    def printFileNames(self):
        for name, path in self.files_dict.iteritems():
            print 'Node', name, ':', path

    def makeDataPathsRelative(self):
        data_nodes = self.dom.getElementsByTagName("DataFiles")[0].childNodes
        xdf_base = self.filename.rstrip('.xdf')
        if xdf_base.find('/') >= 0:
            xdf_base = xdf_base.rsplit('/', 1)[1]
        for node in data_nodes:
            if node.childNodes:
                fn = node.childNodes[0].nodeValue
                fn = fn.partition(xdf_base)[2]
                node.childNodes[0].nodeValue = xdf_base + fn

    def saveAs(self, name):
        the_file = open(name, "w")
        the_file.write(self.dom.toxml())
        the_file.write('\n')
        the_file.close()

def main():

    the_xdf = XDF(sys.argv[1])
    the_xdf.doLoad()
    the_xdf.makeDataPathsRelative()
    the_xdf.saveAs(sys.argv[1])

if __name__ == '__main__':
    main()
    
