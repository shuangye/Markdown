using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;

namespace Editor
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();

            // InitTestData();
            Preview.Navigated += (sender, e) =>
            {
                mshtml.HTMLDocument htmlDoc = (mshtml.HTMLDocument)((WebBrowser)sender).Document;
                mshtml.IHTMLStyleSheet styleSheet = htmlDoc.createStyleSheet(Path.Combine(Directory.GetCurrentDirectory(), @"github-markdown.css"), 0);
                
            };
        }

        #region Event Handlers

        private void Editor_TextChanged(object sender, TextChangedEventArgs e)
        {
            UpdatePreview();
        }

        #endregion Event Handlers


        [DllImport("Parser.dll", CallingConvention = CallingConvention.Cdecl)]
        //[return: MarshalAs(UnmanagedType.LPStr)] 
        public static extern IntPtr markdown_to_string(StringBuilder content, int extensions, int output_format);

        private void UpdatePreview()
        {
            if (null == Preview)
                return;

            string editorContent = new TextRange(Editor.Document.ContentStart, Editor.Document.ContentEnd).Text;            
            // convert in a separate thread
            IntPtr intPtr = markdown_to_string(new StringBuilder(editorContent), 0, 0);
            string html = HtmlHeader() + Marshal.PtrToStringAnsi(intPtr) + "</body></html>";            
            // can also handle its Navigated event
            Preview.NavigateToString(html);                        
        }

        private string HtmlHeader()
        {
            return @"<html><head><meta http-equiv='Content-Type' content='text/html;charset=UTF-8'>"
                //+ @"<link rel='stylesheet' type='text/css' href='http://kevinburke.bitbucket.org/markdowncss/markdown.css'>"
                + "</head><body>";
        }


        #region Misc methods

        private void InitTestData()
        {
            Preview.NavigateToString(@"<html><body><h1>Preview content - H1</h1><p>段落</p></body></html>");
        }

        #endregion Misc methods
    }
}
