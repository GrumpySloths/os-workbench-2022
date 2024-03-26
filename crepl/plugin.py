from pygments import highlight
from pygments.lexers import get_lexer_by_name
from pygments.formatters import TerminalFormatter
import time
def highlight_code(code, language):
    lexer = get_lexer_by_name(language, stripall=True)
    formatter = TerminalFormatter()
    highlighted_code = highlight(code, lexer, formatter)
    return highlighted_code

# 从管道中读取输入并将其高亮输出
def main():
    import sys
    
    code = sys.stdin.read()
    highlighted_code = highlight_code(code, "c")
    print(highlighted_code)

if __name__ == "__main__":
    main()