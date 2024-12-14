# extract_text_from_docx.py
from docx import Document
import sys

def extract_text_from_docx(file_path):
    doc = Document(file_path)
    text = ""
    for paragraph in doc.paragraphs:
        text += paragraph.text + "\n"
    return text

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python extract_text_from_docx.py <docx_file_path>")
        sys.exit(1)

    docx_file_path = sys.argv[1]
    text_content = extract_text_from_docx(docx_file_path)

    with open("output.txt", "w", encoding="utf-8") as output_file:
        output_file.write(text_content)
