import org.w3c.dom.Document;
import org.w3c.dom.Element;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.transform.OutputKeys;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;
import java.io.File;
import java.io.PrintWriter;
import java.util.Date;

/**
 * Created by Женя on 09.09.2014.
 */
public class XMLPrinter {
    private String name;
    private DocumentBuilderFactory dbf;
    private DocumentBuilder db;
    private Document mainDoc;
    private Integer id;
    public XMLPrinter(String name) {
        id = 0;
        this.name = name;
        dbf = DocumentBuilderFactory.newInstance();
        try {
            db = dbf.newDocumentBuilder();
        } catch (ParserConfigurationException e) {
            e.printStackTrace();
        }
        mainDoc = db.newDocument();
        Element leagues = mainDoc.createElement("leagues");
        leagues.setAttribute("number", "null");
        mainDoc.appendChild(leagues);
    }

    public void addElement(Element el, String name, String text) {
        if (text.equals("ud"))
            return;
        Element newEl = mainDoc.createElement(name);
        newEl.setTextContent(text);
        el.appendChild(newEl);
    }

    private Long makeDate(String time) {
        long ans = 0;
        time = time.replace('.', '!');
        String[] tmp1 = time.split(" ");
        String[] dmy = tmp1[0].split("!");
        String[] hm = tmp1[1].split(":");
        int year = Integer.parseInt(dmy[2]);
        int month = Integer.parseInt(dmy[1]);
        int day = Integer.parseInt(dmy[0]);
        int hours = Integer.parseInt(hm[0]);
        int minutes = Integer.parseInt(hm[1]);
        ans = Date.UTC(2000 + year - 1900, month, day, hours, minutes, 0);
        return ans / 1000;
    }
    public void addLeague(String leagueName, Match[] league) {
        Element newLeague = mainDoc.createElement("league");
        newLeague.setAttribute("id", id.toString());
        newLeague.setAttribute("name", leagueName);
        for (int i = 0; i < league.length; i++) {
            Element newMatch = mainDoc.createElement("line");
            addElement(newMatch, "time", makeDate(league[i].time).toString());

            addElement(newMatch, "team1", league[i].team1);
            addElement(newMatch, "team2", league[i].team2);

            addElement(newMatch, "win1", league[i].win1);
            addElement(newMatch, "draw", league[i].draw);
            addElement(newMatch, "win2", league[i].win2);

            addElement(newMatch, "win1draw", league[i].win1draw);
            addElement(newMatch, "win1win2", league[i].win1win2);
            addElement(newMatch, "win2draw", league[i].win2draw);

            newLeague.appendChild(newMatch);
        }
        mainDoc.getDocumentElement().appendChild(newLeague);
        id++;
    }
    public void makeXMLFile(Integer cntLeagues) {
        ((Element)mainDoc.getFirstChild()).setAttribute("number", cntLeagues.toString());
        try {
            File f = new File(name + ".xml");
            PrintWriter printer = new PrintWriter(f);
            Transformer transformer = TransformerFactory.newInstance().newTransformer();
            transformer.setOutputProperty(OutputKeys.METHOD, "xml");
            transformer.setOutputProperty(OutputKeys.OMIT_XML_DECLARATION, "yes");
            transformer.setOutputProperty(javax.xml.transform.OutputKeys.INDENT, "yes");
            transformer.setOutputProperty(OutputKeys.INDENT, "yes");
            transformer.setOutputProperty("{http://xml.apache.org/xslt}indent-amount", "2");
            transformer.transform(new DOMSource(mainDoc), new StreamResult(printer));
            printer.close();
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }


}
