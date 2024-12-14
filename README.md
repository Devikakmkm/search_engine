# search_engine
This project is a simple search engine application developed in C++ that indexes and searches documents. 
# overview
> The Search Engine project is a C++ application designed to index and search documents efficiently.
> It supports text files (.txt) and Word documents (.docx) using a combination of tokenization, an inverted index, and the BM25 ranking algorithm.
> This tool provides search history tracking, document management, and the ability to browse queries directly in a web browser.

# Features
1.Document Indexing: Efficiently tokenize and index document content for rapid search operations./ln
2.BM25 Scoring: Relevance ranking using the BM25 algorithm to deliver accurate search results.
3.Search History: Logs user queries for future reference.
4.Browser Search: Redirects user queries to Google in the default web browser.
5.Dynamic Document Loading: Supports .txt and .docx files, with paths defined in a configuration file.

# Prerequisites
C++17 or Newer: Ensure your system has a modern C++ compiler (e.g., GCC, Clang, MSVC).
Python: Required for extracting text from .docx files.
Python Script: Place extract_text_from_docx.py in the project folder.
Windows OS: The ShellExecuteA and Sleep functions currently depend on Windows.

# Installation
> Clone or download the repository to your local system.
> the following files are in the project directory:
  extract_text_from_docx.py (Python script for extracting Word document text).
  config.txt (List of file paths to be indexed).
> Install Python if not already installed, along with any dependencies required by extract_text_from_docx.py.
> Place document files (.txt or .docx) in the paths listed in config.txt.


