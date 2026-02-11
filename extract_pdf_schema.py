#!/usr/bin/env python3
"""
Script to extract database schema from PDF
Run this on your Windows machine with Python installed
"""

import sys
import re

def extract_text_from_pdf(pdf_path):
    """Extract text from PDF using PyPDF2"""
    try:
        import PyPDF2
    except ImportError:
        print("ERROR: PyPDF2 not installed.")
        print("Install it with: pip install PyPDF2")
        sys.exit(1)

    text = []
    with open(pdf_path, 'rb') as file:
        pdf_reader = PyPDF2.PdfReader(file)
        print(f"Total pages: {len(pdf_reader.pages)}")

        # Extract first 50 pages (usually contains table definitions)
        max_pages = min(50, len(pdf_reader.pages))
        for page_num in range(max_pages):
            page = pdf_reader.pages[page_num]
            text.append(page.extract_text())
            if page_num % 10 == 0:
                print(f"Processing page {page_num}...")

    return '\n'.join(text)

def parse_tables(text):
    """Parse table definitions from text"""
    tables = {}

    # Look for common patterns in Access documentation
    table_pattern = r'Table:\s*(\w+)'
    column_pattern = r'(\w+)\s+(Text|Number|Currency|Date|Memo|Yes/No)'

    current_table = None

    for line in text.split('\n'):
        # Check for table name
        table_match = re.search(table_pattern, line)
        if table_match:
            current_table = table_match.group(1)
            tables[current_table] = {'columns': []}
            print(f"Found table: {current_table}")

        # Check for column definition
        if current_table:
            column_match = re.search(column_pattern, line)
            if column_match:
                col_name = column_match.group(1)
                col_type = column_match.group(2)
                tables[current_table]['columns'].append({
                    'name': col_name,
                    'type': col_type
                })

    return tables

def generate_markdown(tables):
    """Generate markdown output"""
    output = ["# Database Schema Extracted from PDF\n"]

    for table_name, table_info in tables.items():
        output.append(f"\n## Table: {table_name}\n")
        output.append("| Column Name | Data Type |")
        output.append("|-------------|-----------|")

        for col in table_info['columns']:
            output.append(f"| {col['name']} | {col['type']} |")

        output.append("")

    return '\n'.join(output)

def main():
    if len(sys.argv) < 2:
        print("Usage: python extract_pdf_schema.py <pdf_file>")
        print("Example: python extract_pdf_schema.py doc_rptObjects.pdf")
        sys.exit(1)

    pdf_path = sys.argv[1]

    print(f"Extracting text from {pdf_path}...")
    text = extract_text_from_pdf(pdf_path)

    print("\nParsing tables...")
    tables = parse_tables(text)

    print(f"\nFound {len(tables)} tables")

    markdown = generate_markdown(tables)

    output_file = "database_schema_extracted.md"
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(markdown)

    print(f"\nSchema saved to: {output_file}")
    print("\nPlease review and edit the file to add:")
    print("- Missing columns")
    print("- Primary keys")
    print("- Foreign keys")
    print("- Relationships")

if __name__ == "__main__":
    main()
