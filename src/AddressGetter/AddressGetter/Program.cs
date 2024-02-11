using System.Text.Json;

public class ScriptMethod
{
    public int Address { get; set; }
    public string Name { get; set; }
    public string Signature { get; set; }
    public string TypeSignature { get; set; }
}

class Program
{
    static void Main(string[] args)
    {
        string inputFile;
        string searchFor;
        string outputFile;
        DumpedFunctionClass dumpedFunctionClass;

        if (args.Length < 1)
        {
            Console.WriteLine("Please enter the path to the script.json file:");
            inputFile = Console.ReadLine();

            Console.WriteLine(
                "Please enter the method name you want to search for (seperatew by a comma if you want to search for multiple methods):");
            searchFor = Console.ReadLine();

            Console.WriteLine("Please enter the output file path:");
            outputFile = Console.ReadLine();

            //Remove all double quotes 
            inputFile = inputFile.Replace("\"", "");
            outputFile = outputFile.Replace("\"", "");
            searchFor = searchFor.Replace("\"", "");
        }
        else
        {
            inputFile = args[0];
            searchFor = args[1];
            outputFile = args[2];
        }

        dumpedFunctionClass =
            JsonSerializer.Deserialize<DumpedFunctionClass>(
                File.ReadAllText(inputFile));

        List<string> methods = new();
        foreach (var method in dumpedFunctionClass.ScriptMethod)
        {
            foreach (var mn in searchFor.Split(','))
            {
                if (method.Name.ToLower().Contains(mn.ToLower()))
                {
                    methods.Add(method.Name + "=" + "0x" + method.Address);
                    Console.WriteLine("Method: " + method.Name + "\nMethod Address: " + method.Address);
                }
            }
        }

        if (methods.Count < 1) Console.WriteLine("Method not found!");

        if (!string.IsNullOrWhiteSpace(outputFile))
        {
            File.WriteAllLines(outputFile, methods);
        }

        Console.ReadKey();
    }
}

public class DumpedFunctionClass
{
    public ScriptMethod[] ScriptMethod { get; set; }
}
