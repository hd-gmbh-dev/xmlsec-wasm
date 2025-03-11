import fs from 'fs'
import { describe, it } from "vitest";
import XmlSecWasm from "../out/pkg/xmlsec-wasm.js"

describe("xml signer", async () => {
  const m = await XmlSecWasm({})

  it("should be able to create a xml signer instance", async () => {
    const s = new m.XmlSignature();
    const keystoreFileContent = fs.readFileSync('./tests/fixtures/store.p12')
    const buf = new m.Buffer(new Uint8Array(keystoreFileContent))
    const keystore = s.loadPKCS12(buf);
    expect(keystore.isValid()).eq(true)
    const signer = keystore.signer('iaogun2ZvjD3BVzTq7761e');
    expect(signer.isValid()).eq(true)
    const template = `<?xml version="1.0" encoding="UTF-8"?>
<!--
XML Security Library example: Simple signature template file for sign1 example.
-->
<Envelope xmlns="urn:envelope">
  <Data>
	Hello, World!
  </Data>
  <Signature xmlns="http://www.w3.org/2000/09/xmldsig#">
    <SignedInfo>
      <CanonicalizationMethod Algorithm="http://www.w3.org/TR/2001/REC-xml-c14n-20010315" />
      <SignatureMethod Algorithm="http://www.w3.org/2000/09/xmldsig#rsa-sha1" />
      <Reference URI="">
        <Transforms>
          <Transform Algorithm="http://www.w3.org/2000/09/xmldsig#enveloped-signature" />
        </Transforms>
        <DigestMethod Algorithm="http://www.w3.org/2000/09/xmldsig#sha1" />
        <DigestValue></DigestValue>
      </Reference>
    </SignedInfo>
    <SignatureValue/>
    <KeyInfo>
	    <KeyName/>
      <X509Data>
        <X509Certificate></X509Certificate>
      </X509Data>
    </KeyInfo>
  </Signature>
</Envelope>`;
    const result = signer.sign(template);
    const v = s.validator();
    v.load(new m.Buffer(new Uint8Array(fs.readFileSync('./tests/fixtures/test_root_ca.cert'))));
    v.load(new m.Buffer(new Uint8Array(fs.readFileSync('./tests/fixtures/test_ca.cert'))));
    expect(v.verify(result.content())).eq(true)
    v.delete()
    signer.delete();
    keystore.delete()
    s.delete();
  });
});
