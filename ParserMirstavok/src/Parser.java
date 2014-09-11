
import javafx.application.Application;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.concurrent.Worker;
import javafx.scene.*;
import javafx.scene.web.WebEngine;
import javafx.scene.web.WebView;
import javafx.stage.Stage;
import org.w3c.dom.*;
import org.w3c.dom.Node;
import org.w3c.dom.html.HTMLScriptElement;

import javax.imageio.metadata.IIOMetadataNode;
import javax.print.Doc;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.soap.SOAPPart;
import javax.xml.transform.OutputKeys;
import javax.xml.transform.Result;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;
import java.io.File;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;

/**
 * Created by Женя on 07.09.2014.
 */

public class Parser extends Application {

    public static void main(String[] args) {
        launch(args);
    }
    public WebView view;
    public WebEngine engine;
    DocumentBuilderFactory dbf;
    DocumentBuilder db;
    XMLPrinter xmlprinter;
    public Parser() {
        super();
        xmlprinter = new XMLPrinter("mirstavok");
        view = new WebView();
        engine = view.getEngine();
        dbf = DocumentBuilderFactory.newInstance();
        try {
            db = dbf.newDocumentBuilder();
        } catch (ParserConfigurationException e) {
            e.printStackTrace();
        }
    }
    @Override
    public void start(final Stage primaryStage) throws Exception {
        engine.getLoadWorker().stateProperty().addListener(
                new ChangeListener<Worker.State>() {
                    @Override
                    public void changed(ObservableValue<? extends Worker.State> observable, Worker.State oldValue, Worker.State newValue) {
                        if (newValue == Worker.State.SUCCEEDED) {
                            Document doc = engine.getDocument();
                            whenLoaded(doc);
                        }
                    }
                }
        );
        engine.load("http://www.mirstavok.com/line/#viewline*0*");
    }
    public boolean haveAttribute(NamedNodeMap atr, String atrName, String atrValue) {
        if (atr != null && atr.getNamedItem(atrName) != null && atr.getNamedItem(atrName).getNodeValue().matches(atrValue))
            return true;
        return false;
    }
    public String getAttribute(NamedNodeMap atr, String name) {
        if (atr != null && atr.getNamedItem(name) != null)
            return  atr.getNamedItem(name).getNodeValue();
        return null;
    }
    public void findNodesByAttribute(NodeList list, String atrName, String atrValue, Document root) {
        if (list == null)
            return;
        for (int i = 0; i < list.getLength(); i++) {
            Node childItem = list.item(i);
            if (haveAttribute(childItem.getAttributes(), atrName, atrValue)) {
                Node tmp = root.importNode(childItem, true);
                root.getFirstChild().appendChild(tmp);
            }
            else {
                findNodesByAttribute(childItem.getChildNodes(), atrName, atrValue, root);
            }
        }
        return;
    }
    public Document select(Document doc, String atrName, String atrValue) {
        Document root = db.newDocument();
        Element child = root.createElement("html");
        root.appendChild(child);
        root.setNodeValue("html");
        findNodesByAttribute(doc.getChildNodes(), atrName, atrValue, root);
        return root;
    }
    public Document makeDocumentFromNode(Node v) {
        Document root = db.newDocument();
        v = root.importNode(v, true);
        root.appendChild(v);
        return root;
    }
    public Match parseMatch(Document doc) {
        Match res = new Match();
        res.time = doc.getElementsByTagName("I").item(0).getTextContent();
        String[] tmp = doc.getElementsByTagName("STRONG").item(0).getTextContent().split(" - ");
        res.team1 = tmp[0];
        res.team2 = tmp[1];
        String[] k = new String[13];
        for (int i = 5, c = 0; c < 13 && i < doc.getFirstChild().getChildNodes().getLength(); i += 2, c++) {
            k[c] = doc.getFirstChild().getChildNodes().item(i).getTextContent().trim();
            if (k[c].charAt(0) == (char)160)
                k[c] = "ud";
        }
        res.win1 = k[0];
        res.draw = k[1];
        res.win2 = k[2];
        res.win1draw = k[3];
        res.win1win2 = k[4];
        res.win2draw = k[5];
        res.total = k[10];
        res.m = k[11];
        res.b = k[12];
        return res;
    }

    public void parseItemData(Document doc, String leagueName) {
        doc = select(doc, "id", "event.*");
        int n = doc.getFirstChild().getChildNodes().getLength();
        Match[] league = new Match[n];
        for (int i = 0; i < n; ++i) {
            league[i] = parseMatch(makeDocumentFromNode(doc.getFirstChild().getChildNodes().item(i)));
        }
        xmlprinter.addLeague(leagueName, league);
    }
    public void whenLoaded(Document doc) {
        doc = select(doc, "class", "item exitem uncollapse");
        NodeList items = doc.getFirstChild().getChildNodes();
        int leagues = 0;
        for (int i = 0; i < items.getLength(); i++) {
            Document nowDoc = makeDocumentFromNode(items.item(i));
            Document tmp = select(nowDoc, "class", "shadowed");
            String leagueName = tmp.getFirstChild().getFirstChild().getTextContent();
            if (!leagueName.toLowerCase().startsWith("футбол"))
                continue;
            parseItemData(select(nowDoc, "class", "itemdata"), leagueName);
            ++leagues;
        }
        xmlprinter.makeXMLFile(leagues);
        System.exit(0);
    }
    public void printDoc(Document doc) {
        try {
            File f = new File("html.txt");
            PrintWriter printer = new PrintWriter(f);
            Transformer transformer = TransformerFactory.newInstance().newTransformer();
            transformer.setOutputProperty(OutputKeys.OMIT_XML_DECLARATION, "yes");
            transformer.transform(new DOMSource(doc), new StreamResult(System.out));
            printer.close();

        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }


}
