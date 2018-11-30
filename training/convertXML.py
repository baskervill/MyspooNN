from xml.etree import ElementTree as etree
from xml.etree.ElementTree import Element, SubElement, ElementTree
from xml.dom import minidom
import os, shutil


def prettify(elem):
    """Return a pretty-printed XML string for the Element.
    """
    rough_string = etree.tostring(elem, 'utf-8')
    reparsed = minidom.parseString(rough_string)
    return reparsed.toprettyxml(indent="\t")


def openfile(filename):
    with open(filename, 'r') as fin:
        first_line = fin.readline().strip('\n').split(' ')
        label = []
        if len(first_line) == 5:
            label.append(int(first_line[0]))
            label.append(int(float(first_line[1]) * 1024))
            label.append(int(float(first_line[2]) * 1024))
            label.append(int(float(first_line[3]) * 1024))
            label.append(int(float(first_line[4]) * 1024))
            return label
        else:
            return None

if __name__ == '__main__':
    img_dir = '/home/haoyunchao/WORK/PYNQ/ship/JPEGImages'
    label_dir = '/home/haoyunchao/WORK/PYNQ/ship/labels'
    xml_dir = '/home/haoyunchao/WORK/PYNQ/ship/xml'
    for f in os.listdir(label_dir):
        if 'txt' in f:
            label = openfile(os.path.join(label_dir, f))
            if label is not None:
                target_name = f.replace('txt', 'xml')
                annotation = Element('annotation')
                object_fild = SubElement(annotation, 'object')
                name = SubElement(object_fild, 'name')
                name.text = str(label[0])
                bndbox = SubElement(object_fild, 'bndbox')
                xmin = SubElement(bndbox, 'xmin')
                xmin.text = str(label[1])
                ymin = SubElement(bndbox, 'ymin')
                ymin.text = str(label[2])
                xmax = SubElement(bndbox, 'xmax')
                xmax.text = str(label[1] + label[3])
                ymax = SubElement(bndbox, 'ymax')
                ymax.text = str(label[2] + label[4])
                xml_string = prettify(annotation)
                # print(xml_string)
                with open(os.path.join(xml_dir, target_name), 'w') as writing:
                    writing.write(xml_string)
